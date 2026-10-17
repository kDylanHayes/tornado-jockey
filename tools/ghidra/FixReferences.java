// Correct operand references before delinking.
//
// 1. Indexed operands like [EAX*4 + 0x583e33]: constant propagation can
//    leave Ghidra's reference on table + index*4 (0x583e63) instead of the
//    table itself, and the delinker then emits no relocation for the
//    instruction. They're re-pointed at the displacement.
// 2. Constants that only look like addresses (a flag mask of 0x600080, a
//    0x400000 limit): every address listed in config/norefs.txt is the
//    location of such a constant, and references to its value are removed
//    so no relocation is emitted.
// Usage: run as a -preScript before DelinkProgram.java (see tools/delink.py).
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.symbol.SourceType;

public class FixReferences extends HeadlessScript {
	@Override
	public void run() throws Exception {
		ReferenceManager refs = currentProgram.getReferenceManager();
		int fixed = 0;
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
				printerr("No instruction at " + loc);
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
