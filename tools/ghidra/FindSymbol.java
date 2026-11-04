// Print the address of every symbol whose name contains one of the given
// substrings, with the bytes at that address.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis -readOnly \
//          -scriptPath tools/ghidra -postScript FindSymbol.java <substring> [<substring> ...]
//@category TornadoJockey

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

public class FindSymbol extends HeadlessScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		SymbolTable table = currentProgram.getSymbolTable();
		for (String want : args) {
			int found = 0;
			SymbolIterator it = table.getAllSymbols(true);
			while (it.hasNext()) {
				Symbol s = it.next();
				if (!s.getName().contains(want)) {
					continue;
				}
				Address a = s.getAddress();
				StringBuilder hex = new StringBuilder();
				byte[] b = new byte[12];
				try {
					currentProgram.getMemory().getBytes(a, b);
					for (byte x : b) {
						hex.append(String.format("%02x ", x));
					}
				}
				catch (Exception e) {
					hex.append("?");
				}
				println(String.format("%s  %s  [%s]  %s", a, s.getName(), hex.toString().trim(),
					s.getSymbolType()));
				found++;
			}
			if (found == 0) {
				println(want + "  NOT FOUND");
			}
		}
	}
}
