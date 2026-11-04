// Report functions that contain instruction bytes no 2006 C++ compiler emits
// (AAD, AAM, BOUND, CLI, STI, HLT, INT3, SALC, ...). Such runs are the bytes
// the retail image keeps where the launcher normally patches code in, and no
// source can reproduce them, so a function holding one can never reach 100%.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis -readOnly \
//          -scriptPath tools/ghidra -postScript FindJunk.java
//@category TornadoJockey

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class FindJunk extends HeadlessScript {
	private static final String[] JUNK = {
		"AAD", "AAM", "BOUND", "CLI", "STI", "HLT", "INT3", "SALC", "IRET",
		"INTO", "XLAT", "UD2", "ARPL", "LDS", "LES", "LSS", "LFS", "LGS"
	};

	@Override
	public void run() throws Exception {
		FunctionManager functions = currentProgram.getFunctionManager();
		int flagged = 0;
		int total = 0;
		for (Function function : functions.getFunctions(true)) {
			total++;
			StringBuilder hits = new StringBuilder();
			int count = 0;
			InstructionIterator it = currentProgram.getListing()
				.getInstructions(function.getBody(), true);
			for (Instruction ins : it) {
				String m = ins.getMnemonicString().toUpperCase();
				for (String j : JUNK) {
					if (m.equals(j)) {
						hits.append("    ").append(ins.getAddress()).append("  ")
							.append(ins.toString()).append("\n");
						count++;
						break;
					}
				}
			}
			if (count > 0) {
				println(function.getName() + " @ " + function.getEntryPoint()
					+ " (" + function.getBody().getNumAddresses() + " bytes, "
					+ count + " junk)");
				print(hits.toString());
				flagged++;
			}
		}
		println("// " + flagged + " of " + total + " functions hold junk bytes");
	}
}
