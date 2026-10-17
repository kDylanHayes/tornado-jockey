// Point indexed memory operands at their displacement.
// For an operand like [EAX*4 + 0x583e33], constant propagation can leave
// Ghidra's reference on table + index*4 (0x583e63) instead of the table
// itself. The delinker then emits no relocation for the instruction, so
// the operand is re-pointed at the displacement before delinking.
// Usage: run as a -preScript before DelinkProgram.java (see tools/delink.py).
//@category TornadoJockey

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;
import ghidra.program.model.symbol.SourceType;

public class FixIndexedRefs extends HeadlessScript {
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
					} else if (obj instanceof Scalar s) {
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
					if (!ref.isMemoryReference() || ref.getReferenceType().isFlow()
							|| ref.getToAddress().equals(disp)) {
						continue;
					}
					RefType type = ref.getReferenceType();
					refs.delete(ref);
					refs.addMemoryReference(ins.getAddress(), disp, type, SourceType.USER_DEFINED, op)
							.getReferenceType();
					fixed++;
				}
			}
		}
		println("Re-pointed " + fixed + " indexed operand references");
	}
}
