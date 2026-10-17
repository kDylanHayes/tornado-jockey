// Write "<function address>\t<string address>\t<string>" for every defined
// string referenced from code. Used to locate libraries and source files.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis \
//          -scriptPath tools/ghidra -postScript ExportStringRefs.java <out.tsv>
//@category TornadoJockey

import java.io.PrintWriter;

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.DataIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class ExportStringRefs extends HeadlessScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		int count = 0;
		try (PrintWriter out = new PrintWriter(args[0])) {
			DataIterator it = currentProgram.getListing().getDefinedData(true);
			while (it.hasNext()) {
				Data d = it.next();
				if (!d.hasStringValue()) {
					continue;
				}
				String value = String.valueOf(d.getValue()).replace('\t', ' ').replace('\n', ' ').replace('\r', ' ');
				for (Reference ref : getReferencesTo(d.getAddress())) {
					Function f = getFunctionContaining(ref.getFromAddress());
					if (f == null) {
						continue;
					}
					out.printf("0x%08x\t0x%08x\t%s%n", f.getEntryPoint().getOffset(), d.getAddress().getOffset(), value);
					count++;
				}
			}
		}
		println("Wrote " + count + " string references to " + args[0]);
	}
}
