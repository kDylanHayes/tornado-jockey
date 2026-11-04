// Report functions whose body Ghidra did not fully disassemble, with the size
// of each gap. The delinker copies those bytes without relocations, so calls
// and data references inside them can never match.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis -readOnly \
//          -scriptPath tools/ghidra -postScript FindGaps.java [min_gap]
//@category TornadoJockey

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Instruction;

public class FindGaps extends HeadlessScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		long minGap = args.length > 0 ? Long.parseLong(args[0]) : 1;
		FunctionManager functions = currentProgram.getFunctionManager();
		int flagged = 0;
		int total = 0;
		long totalGap = 0;
		for (Function f : functions.getFunctions(true)) {
			total++;
			AddressSetView body = f.getBody();
			long gap = 0;
			long biggest = 0;
			StringBuilder where = new StringBuilder();
			Address a = body.getMinAddress();
			Address end = body.getMaxAddress();
			while (a != null && a.compareTo(end) <= 0) {
				Instruction ins = getInstructionAt(a);
				if (ins == null) {
					long run = 0;
					Address start = a;
					while (a.compareTo(end) <= 0 && getInstructionAt(a) == null) {
						run++;
						a = a.add(1);
					}
					gap += run;
					if (run > biggest) {
						biggest = run;
					}
					if (run >= minGap) {
						where.append(" ").append(start).append("(").append(run).append(")");
					}
				}
				else {
					a = ins.getMaxAddress().add(1);
				}
			}
			if (gap > 0) {
				totalGap += gap;
				if (gap >= minGap) {
					println(String.format("%s @ %s  size=%d gap=%d%s", f.getName(), f.getEntryPoint(),
						body.getNumAddresses(), gap, where.toString()));
					flagged++;
				}
			}
		}
		println("// " + flagged + " of " + total + " functions have a gap of >= " + minGap
			+ " (total gap bytes " + totalGap + ")");
	}
}
