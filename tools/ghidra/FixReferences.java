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
// Usage: run as a -preScript before DelinkUnit.java (see tools/delink.py).
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;

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

	@Override
	public void run() throws Exception {
		ReferenceManager refs = currentProgram.getReferenceManager();
		int fixed = 0;
		int added = 0;
		int cleared = 0;
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
				}
				if (!indexed && disp != null && looksLikeString(disp)) {
					clearListing(disp, disp.add(3));
					cleared++;
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
			}
		}
		println("Re-pointed " + fixed + " indexed operand references");
		println("Added " + added + " references from immediates into data");
		println("Cleared " + cleared + " short strings typed as pointers");

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
