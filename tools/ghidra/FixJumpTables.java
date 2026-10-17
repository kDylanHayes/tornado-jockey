// Type every entry of a switch jump table as a pointer before delinking.
//
// When analysis can't recover a switch (`jmp [reg*4 + table]`), the table's
// entries are typed only where pointer scanning happened to find them, and an
// entry left as raw bytes gets no relocation. For each such jump the table
// size comes from the bounds check before it (`cmp reg, N` / `ja`), or else
// runs while entries point back into the function ahead of the table.
// Usage: run as a -preScript before DelinkUnit.java (see tools/delink.py).
//@category TornadoJockey

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.PointerDataType;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.scalar.Scalar;

public class FixJumpTables extends HeadlessScript {
	private static final int MAX_ENTRIES = 1024;

	// N + 1 for a `cmp index, N` a few instructions before the jump, else -1.
	// A two-level switch loads the index from a byte table after the check
	// (`movzx index, byte ptr [x + bytes]`), so the bound isn't the size of
	// this table; it gets -1 too.
	private int boundedCount(Instruction jmp, Register index) {
		Instruction ins = jmp;
		for (int back = 0; back < 8 && ins != null; back++) {
			ins = ins.getPrevious();
			if (ins == null) {
				return -1;
			}
			if (!ins.getMnemonicString().equals("CMP")) {
				for (Object o : ins.getResultObjects()) {
					if (o instanceof Register r && r.contains(index)) {
						return -1;
					}
				}
				continue;
			}
			Object[] lhs = ins.getOpObjects(0);
			Object[] rhs = ins.getOpObjects(1);
			if (lhs.length == 1 && lhs[0].equals(index) && rhs.length == 1 && rhs[0] instanceof Scalar s) {
				return (int) s.getUnsignedValue() + 1;
			}
			return -1;
		}
		return -1;
	}

	@Override
	public void run() throws Exception {
		Memory mem = currentProgram.getMemory();
		int typed = 0;
		for (Instruction ins : currentProgram.getListing().getInstructions(true)) {
			if (!ins.getMnemonicString().equals("JMP") || ins.getNumOperands() != 1) {
				continue;
			}
			Register index = null;
			Address table = null;
			for (Object obj : ins.getOpObjects(0)) {
				if (obj instanceof Register r) {
					index = r;
				}
				else if (obj instanceof Scalar s) {
					table = toAddr(s.getUnsignedValue());
				}
			}
			MemoryBlock block = table == null ? null : mem.getBlock(table);
			if (index == null || block == null || !block.isExecute()) {
				continue;
			}
			Function owner = getFunctionContaining(ins.getAddress());
			Address low = owner != null ? owner.getEntryPoint() : ins.getAddress();
			int count = boundedCount(ins, index);
			for (int i = 0; i < (count < 0 ? MAX_ENTRIES : count); i++) {
				Address entry = table.add(4L * i);
				Address target = toAddr(mem.getInt(entry) & 0xffffffffL);
				if (count < 0 && (target.compareTo(low) < 0 || target.compareTo(table) >= 0)) {
					break;
				}
				Data d = getDataAt(entry);
				if (d != null && d.isPointer()) {
					continue;
				}
				clearListing(entry, entry.add(3));
				createData(entry, PointerDataType.dataType);
				typed++;
			}
		}
		println("Typed " + typed + " jump table entries as pointers");
	}
}
