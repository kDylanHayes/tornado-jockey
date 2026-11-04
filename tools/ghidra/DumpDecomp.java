// Print the decompiler's C for every function that intersects the given
// address ranges. Output is plain text with a header naming each function and
// its address; --out writes it to a file (headless adds log prefixes to
// stdout), otherwise it goes to the script log.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis -readOnly \
//          -scriptPath tools/ghidra -postScript DumpDecomp.java \
//          <start-end> [<start-end> ...] [--out <file.c>]
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;

public class DumpDecomp extends HeadlessScript {
	private static final int TIMEOUT_SECONDS = 120;

	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		AddressSet set = new AddressSet();
		String out = null;
		int rangeCount = 0;
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("--out")) {
				if (++i >= args.length) {
					throw new IllegalArgumentException("--out needs a file name");
				}
				out = args[i];
				continue;
			}
			String[] range = args[i].split("-", 2);
			if (range.length != 2) {
				throw new IllegalArgumentException("range must be start-end: " + args[i]);
			}
			Address start = toAddr(range[0]);
			Address end = toAddr(range[1]);
			if (start == null || end == null) {
				throw new IllegalArgumentException("bad address in range: " + args[i]);
			}
			set.add(start, end);
			rangeCount++;
		}
		if (set.isEmpty()) {
			throw new IllegalArgumentException("pass one or more start-end ranges");
		}
		StringBuilder text = new StringBuilder();

		DecompInterface decompiler = new DecompInterface();
		decompiler.toggleCCode(true);
		decompiler.toggleSyntaxTree(true);
		if (!decompiler.openProgram(currentProgram)) {
			throw new RuntimeException("decompiler could not open the program");
		}

		FunctionManager functions = currentProgram.getFunctionManager();
		int count = 0;
		try {
			for (Function function : functions.getFunctions(true)) {
				// Include functions that start before a range and reach into it.
				if (!function.getBody().intersects(set)) {
					continue;
				}
				long size = function.getBody().getNumAddresses();
				text.append("// ==== ").append(function.getName()).append(" @ ")
					.append(function.getEntryPoint()).append(" (").append(size)
					.append(" bytes) ====\n");
				DecompileResults results =
					decompiler.decompileFunction(function, TIMEOUT_SECONDS, monitor);
				if (results.decompileCompleted() && results.getDecompiledFunction() != null) {
					text.append(results.getDecompiledFunction().getC());
				} else {
					text.append("// decompilation failed: ")
						.append(results.getErrorMessage());
				}
				text.append("\n\n");
				count++;
			}
		} finally {
			decompiler.dispose();
		}
		text.append("// ").append(count).append(" function(s) in ")
			.append(rangeCount).append(" range(s)\n");

		if (out != null) {
			Files.writeString(Paths.get(out), text.toString());
			println("Wrote " + count + " function(s) to " + out);
		} else {
			println(text.toString());
		}
	}
}
