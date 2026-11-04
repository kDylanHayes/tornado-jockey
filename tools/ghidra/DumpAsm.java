// Print the disassembly for every function that intersects the given address
// ranges, one instruction per line, with operands as Ghidra renders them
// (direct calls and data references show the symbol they resolve to).
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis -readOnly \
//          -scriptPath tools/ghidra -preScript ApplySymbols.java <symbols.txt> \
//          -postScript DumpAsm.java <start-end> [<start-end> ...] [--out <file.asm>]
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class DumpAsm extends HeadlessScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		AddressSet set = new AddressSet();
		String out = null;
		boolean fix = false;
		int rangeCount = 0;
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("--out")) {
				if (++i >= args.length) {
					throw new IllegalArgumentException("--out needs a file name");
				}
				out = args[i];
				continue;
			}
			if (args[i].equals("--fix")) {
				fix = true;
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
		FunctionManager functions = currentProgram.getFunctionManager();
		int count = 0;
		if (fix) {
			Address a = set.getMinAddress();
			Address stop = set.getMaxAddress();
			while (a.compareTo(stop) <= 0) {
				boolean ok = disassemble(a);
				Instruction made = ok ? getInstructionAt(a) : null;
				a = made != null ? made.getMaxAddress().add(1) : a.add(1);
			}
		}
		for (Function function : functions.getFunctions(true)) {
			if (!function.getBody().intersects(set)) {
				continue;
			}
			long size = function.getBody().getNumAddresses();
			text.append("// ==== ").append(function.getName()).append(" @ ")
				.append(function.getEntryPoint()).append(" (").append(size)
				.append(" bytes) ====\n");
			InstructionIterator it = currentProgram.getListing()
				.getInstructions(function.getBody(), true);
			for (Instruction ins : it) {
				String refs = "";
				for (var ref : ins.getReferencesFrom()) {
					var to = ref.getToAddress();
					var sym = getSymbolAt(to);
					String name = sym != null ? sym.getName() : null;
					if (name != null && !refs.contains(name)) {
						refs += "  ; " + name;
					}
				}
				text.append(String.format("%-10s %-8s %-40s%s%n",
					ins.getAddress().toString(), ins.getMnemonicString(), ins.toString(), refs));
			}
			text.append("\n");
			count++;
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
