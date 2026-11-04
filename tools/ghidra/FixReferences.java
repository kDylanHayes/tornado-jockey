// Correct operand references before delinking.
//
// 1. Indexed operands like [EAX*4 + 0x583e33]: constant propagation can
//    leave Ghidra's reference on table + index*4 (0x583e63) instead of the
//    table itself, and the delinker then emits no relocation for the
//    instruction. They're re-pointed at the displacement.
// 2. Constants that only look like addresses (a flag mask of 0x600080, a
//    0x400000 limit): every address listed in config/norefs.txt is the
//    location of such a constant (in an instruction, or in data such as
//    the bytes of L"%f"), and references to its value are removed so no
//    relocation is emitted.
// 3. Immediates pointing into data (mov eax, offset "PNG") that analysis
//    left without a reference get one, so the delinker relocates them;
//    so do immediates into their own function (a catch block returning
//    the address where execution continues).
//    When analysis typed a short string as a pointer ("PNG\0" reads as
//    0x00474e50) the pointer is cleared; the delinker emits no relocation
//    to it otherwise.
// 4. Pointers in data to strings that analysis missed: a command table's
//    names ("set" reads as the pointer 0x00746573, so it isn't a string to
//    analysis, and the table entry pointing at it isn't typed either). Short
//    strings typed as pointers that data points at become strings, and
//    undefined aligned words holding a string's address become pointers.
// Usage: run as a -preScript before DelinkUnit.java (see tools/delink.py).
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Map;
import java.util.TreeMap;

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;

public class FixReferences extends HeadlessScript {
	// A pointer whose four bytes are printable text ending in NUL.
	private boolean looksLikeString(Address a) throws Exception {
		Data d = getDataAt(a);
		if (d == null || !d.isPointer()) {
			return false;
		}
		byte[] b = new byte[4];
		currentProgram.getMemory().getBytes(a, b);
		boolean ended = false;
		for (byte x : b) {
			if (x == 0) {
				ended = true;
			}
			else if (ended || x < 0x20 || x > 0x7e) {
				return false;
			}
		}
		return ended && b[0] != 0;
	}

	// Four bytes of printable text ending in NUL, whether or not analysis
	// typed them as a pointer ("set\0" is the address 0x00746573).
	private boolean bytesLookLikeString(Address a) throws Exception {
		byte[] b = new byte[4];
		if (currentProgram.getMemory().getBytes(a, b) != 4) {
			return false;
		}
		boolean ended = false;
		for (byte x : b) {
			if (x == 0) {
				ended = true;
			}
			else if (ended || x < 0x20 || x > 0x7e) {
				return false;
			}
		}
		return ended && b[0] != 0;
	}

	// A pointer to a string, or to a short string typed as a pointer: a
	// command table whose first name pointer reads as text ("xff").
	private boolean pointsAtString(Address a) throws Exception {
		Address value = toAddr(currentProgram.getMemory().getInt(a) & 0xffffffffL);
		if (!currentProgram.getMemory().contains(value)) {
			return false;
		}
		return isString(value);
	}

	// A string, a short string typed as a pointer, or bytes named as a
	// string literal in symbols.txt that analysis left undefined.
	private boolean isString(Address value) throws Exception {
		Data d = getDataAt(value);
		if (d != null && (d.hasStringValue() || looksLikeString(value))) {
			return true;
		}
		ghidra.program.model.symbol.Symbol sym = getSymbolAt(value);
		return sym != null && (sym.getName().startsWith("??_C@_") || sym.getName().startsWith("s_"));
	}

	// Data symbols from symbols.txt with a size, by address: the delinker
	// loses the addend of a reference into the middle of a symbol (it names
	// the containing symbol with addend 0), so those become offset
	// references instead.
	private final TreeMap<Long, Long> sizedData = new TreeMap<>();

	private void loadSymbolSizes(String path) throws Exception {
		for (String line : Files.readAllLines(Paths.get(path))) {
			line = line.strip();
			if (line.isEmpty() || line.startsWith("#")) {
				continue;
			}
			String[] parts = line.split("\\s+");
			if (parts.length < 4 || !parts[2].equals("data")) {
				continue;
			}
			try {
				long addr = Long.decode(parts[0]);
				long size = Long.decode(parts[3]);
				if (size > 0) {
					sizedData.put(addr, size);
				}
			}
			catch (NumberFormatException e) {
				// A size that isn't a plain number; leave it out.
			}
		}
	}

	// A user-named symbol whose own address is exactly this one
	// (SymbolTable.getSymbols also returns symbols whose data covers the
	// address, and analysis leaves DAT_ labels at unnamed addresses).
	private boolean hasExactSymbol(Address a) {
		for (Symbol s : currentProgram.getSymbolTable().getSymbols(a)) {
			if (s.getAddress().equals(a) && s.getSource() == SourceType.USER_DEFINED) {
				return true;
			}
		}
		return false;
	}

	// The exact address of a data symbol with a size in symbols.txt, such as
	// the name array a property table points at. Analysis types a word as a
	// pointer only when it points at a string or a function, so without this
	// the delinker keeps the raw address of such a word and emits no
	// relocation for it.
	private boolean isSizedDataSymbol(Address value) {
		return sizedData.containsKey(value.getOffset());
	}

	// The nearest preceding sized data symbol that strictly contains value.
	private Long sizedBase(long value) {
		Map.Entry<Long, Long> entry = sizedData.floorEntry(value);
		for (int tries = 0; entry != null && tries < 4; tries++) {
			long base = entry.getKey();
			if (base < value && value < base + entry.getValue()) {
				return base;
			}
			entry = sizedData.lowerEntry(base);
		}
		return null;
	}

	@Override
	public void run() throws Exception {
		ReferenceManager refs = currentProgram.getReferenceManager();
		String[] scriptArgs = getScriptArgs();
		if (scriptArgs.length > 1 && Files.exists(Paths.get(scriptArgs[1]))) {
			loadSymbolSizes(scriptArgs[1]);
		}
		println("Loaded " + sizedData.size() + " sized data symbols");
		int fixed = 0;
		int added = 0;
		int cleared = 0;
		int retyped = 0;
		for (Instruction ins : currentProgram.getListing().getInstructions(true)) {
			for (int op = 0; op < ins.getNumOperands(); op++) {
				Address disp = null;
				boolean indexed = false;
				for (Object obj : ins.getOpObjects(op)) {
					if (obj instanceof Register) {
						indexed = true;
					}
					else if (obj instanceof Scalar s) {
						Address a = toAddr(s.getUnsignedValue());
						if (currentProgram.getMemory().contains(a)) {
							disp = a;
						}
					}
					else if (obj instanceof Address a && currentProgram.getMemory().contains(a)) {
						// An absolute memory operand ([0x6bbe38]) reports its
						// address object, not a scalar.
						disp = a;
					}
				}
				if (disp != null && !hasExactSymbol(disp)) {
					// A reference into the middle of a sized data symbol: the
					// delinker names the containing symbol with addend 0 and
					// loses the offset (g_forward.y read as g_forward). An
					// offset reference keeps it.
					MemoryBlock block = currentProgram.getMemory().getBlock(disp);
					Long base = sizedBase(disp.getUnsignedOffset());
					if (block != null && !block.isExecute() && base != null) {
						RefType type = RefType.DATA;
						for (Reference ref : ins.getOperandReferences(op)) {
							if (ref.getToAddress().equals(disp)) {
								type = ref.getReferenceType();
							}
						}
						for (Reference ref : ins.getOperandReferences(op)) {
							if (ref.getToAddress().equals(disp)) {
								refs.delete(ref);
							}
						}
						refs.addOffsetMemReference(ins.getAddress(), toAddr(base), true,
							disp.getUnsignedOffset() - base, type, SourceType.USER_DEFINED, op);
						fixed++;
						continue;
					}
				}
				if (!indexed && disp != null && looksLikeString(disp) && !pointsAtString(disp)) {
					clearListing(disp, disp.add(3));
					cleared++;
				}
				if (!indexed && disp != null && getDataAt(disp) == null) {
					// Analysis can type a string one byte early, over the tail
					// of the preceding item: RENDERER::Render's "Don't render"
					// message began at 0x673d77, inside __real@44160000, so the
					// named symbol at 0x673d78 fell inside a data item and the
					// delinker emitted no relocation for `push 0x673d78`.
					// Re-type the string at the symbol's own address.
					Data containing = getDataContaining(disp);
					ghidra.program.model.symbol.Symbol sym = getSymbolAt(disp);
					if (containing != null && containing.hasStringValue()
							&& containing.getAddress().compareTo(disp) < 0
							&& sym != null && sym.getSource() == SourceType.USER_DEFINED) {
						clearListing(containing.getAddress(), containing.getMaxAddress());
						try {
							createAsciiString(disp);
							retyped++;
						}
						catch (Exception e) {
							// Something else stays defined in the word.
						}
					}
				}
				if (!indexed && disp != null && !ins.getFlowType().isFlow()) {
					// The EH analysis can leave a flow reference on the
					// continuation address a catch block returns.
					for (Reference ref : ins.getOperandReferences(op)) {
						if (ref.getReferenceType().isFlow()) {
							refs.delete(ref);
						}
					}
				}
				if (!indexed && disp != null && ins.getOperandReferences(op).length == 0 &&
					ins.getNumOperands() > 0) {
					MemoryBlock block = currentProgram.getMemory().getBlock(disp);
					Function owner = getFunctionContaining(ins.getAddress());
					boolean sameFunction = owner != null && owner.getBody().contains(disp);
					if (block != null && block.isInitialized() && (!block.isExecute() || sameFunction)) {
						refs.addMemoryReference(ins.getAddress(), disp, RefType.DATA,
							SourceType.USER_DEFINED, op);
						added++;
					}
					continue;
				}
				if (!indexed || disp == null) {
					continue;
				}
				for (Reference ref : refs.getReferencesFrom(ins.getAddress(), op)) {
					if (!ref.isMemoryReference() || ref.getReferenceType().isFlow() ||
						ref.getToAddress().equals(disp)) {
						continue;
					}
					RefType type = ref.getReferenceType();
					refs.delete(ref);
					refs.addMemoryReference(ins.getAddress(), disp, type, SourceType.USER_DEFINED, op);
					fixed++;
				}
				if (ins.getOperandReferences(op).length == 0) {
					// An indexed data operand (mov [eax*4 + table], ...) that
					// analysis never referenced: the exe holds the named
					// symbol's absolute address, but without a reference the
					// delinker emits no relocation for it.
					MemoryBlock block = currentProgram.getMemory().getBlock(disp);
					Symbol sym = getSymbolAt(disp);
					if (block != null && block.isInitialized() && !block.isExecute() &&
						sym != null && sym.getSource() == SourceType.USER_DEFINED) {
						refs.addMemoryReference(ins.getAddress(), disp, RefType.DATA,
							SourceType.USER_DEFINED, op);
						added++;
					}
				}
			}
		}
		println("Re-pointed " + fixed + " indexed operand references");
		println("Added " + added + " references from immediates into data");
		println("Cleared " + cleared + " short strings typed as pointers");
		println("Re-typed " + retyped + " strings analysis started before their symbol");

		int strings = 0;
		int pointers = 0;
		for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
			if (block.isExecute() || !block.isInitialized()) {
				continue;
			}
			long start = (block.getStart().getOffset() + 3) & ~3L;
			for (long off = start; off + 4 <= block.getEnd().getOffset() + 1; off += 4) {
				Address at = toAddr(off);
				Address value = toAddr(currentProgram.getMemory().getInt(at) & 0xffffffffL);
				if (!currentProgram.getMemory().contains(value) || value.equals(at)) {
					continue;
				}
				if (looksLikeString(value) && getDataContaining(at) != null && getDataContaining(at).isPointer()) {
					clearListing(value, value.add(3));
					createAsciiString(value);
					strings++;
				}
				// A string's address, a function's (a function pointer table
				// or global that analysis didn't type), or a sized data
				// symbol's (a property table's array pointer).
				if (!isString(value) && !isSizedDataSymbol(value) && getFunctionAt(value) == null) {
					continue;
				}
				Data here = getDataContaining(at);
				if (here == null || !here.isDefined()) {
					if (refs.getReferencesFrom(at).length == 0) {
						try {
							createData(at, ghidra.program.model.data.PointerDataType.dataType);
							pointers++;
						} catch (ghidra.program.model.util.CodeUnitInsertionException e) {
							// Something else is defined in the word.
						}
					}
				}
			}
		}
		println("Made " + strings + " short strings pointed to from data strings");
		println("Typed " + pointers + " words in data holding a string's, function's or data symbol's address as pointers");

		String[] args = getScriptArgs();
		if (args.length == 0 || !Files.exists(Paths.get(args[0]))) {
			return;
		}
		int removed = 0;
		for (String line : Files.readAllLines(Paths.get(args[0]))) {
			line = line.strip();
			if (line.isEmpty() || line.startsWith("#")) {
				continue;
			}
			Address loc = toAddr(line.split("\\s+")[0]);
			Instruction ins = getInstructionContaining(loc);
			if (ins == null) {
				// Data that only looks like a pointer (the bytes of L"%f").
				Data d = getDataAt(loc);
				if (d != null && d.isPointer()) {
					clearListing(loc, loc.add(3));
				}
				for (Reference ref : refs.getReferencesFrom(loc)) {
					refs.delete(ref);
					removed++;
				}
				// A short string whose bytes read as an address ("set" is
				// 0x00746573): leave it typed as a string so the relocation
				// synthesizer does not put a relocation on its bytes.
				if (bytesLookLikeString(loc)) {
					try {
						createAsciiString(loc, 4);
					}
					catch (Exception e) {
						// Something else is defined in the word.
					}
				}
				continue;
			}
			Address value = toAddr(currentProgram.getMemory().getInt(loc) & 0xffffffffL);
			for (Reference ref : refs.getReferencesFrom(ins.getAddress())) {
				if (ref.isMemoryReference() && ref.getToAddress().equals(value)) {
					refs.delete(ref);
					removed++;
				}
			}
		}
		println("Removed " + removed + " references to constants");
	}
}
