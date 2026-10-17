// Apply names from config/symbols.txt to the program, so delinked objects
// carry the same symbol names as our compiled objects.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis \
//          -scriptPath tools/ghidra -postScript ApplySymbols.java config/symbols.txt
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class ApplySymbols extends HeadlessScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		int applied = 0;
		for (String line : Files.readAllLines(Paths.get(args[0]))) {
			line = line.strip();
			if (line.isEmpty() || line.startsWith("#")) {
				continue;
			}
			String[] parts = line.split("\\s+");
			if (parts.length < 2 || parts[1].contains("::") || parts[1].startsWith("FUN_")) {
				// Leave namespaced and auto-generated names to Ghidra.
				continue;
			}
			Address addr = toAddr(parts[0]);
			String name = parts[1];
			Function f = getFunctionAt(addr);
			if (f != null) {
				f.setName(name, SourceType.USER_DEFINED);
			}
			else {
				createLabel(addr, name, true, SourceType.USER_DEFINED);
			}
			applied++;
		}
		println("Applied " + applied + " symbols");
	}
}
