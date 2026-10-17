// Export address ranges of the program as a COFF relocatable object, using the
// ghidra-delinker-extension. Unlike the extension's DelinkProgram.java, this
// keeps PC-relative relocations between functions of the same unit (calls from
// one function to another), so tools/split_functions.py can give each function
// its own section the way /Gy objects have them.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis -readOnly \
//          -scriptPath tools/ghidra -postScript DelinkUnit.java \
//          <out.obj> <start-end> [<start-end> ...] [-- <out.obj> <start-end> ...]
//@category TornadoJockey

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import ghidra.app.plugin.core.analysis.AutoAnalysisManager;
import ghidra.app.util.Option;
import ghidra.app.util.exporter.Exporter;
import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.AddressSet;
import ghidra.util.classfinder.ClassSearcher;

public class DelinkUnit extends HeadlessScript {
	private static final String EXPORTER = "COFF relocatable object";
	private static final String TRIM_OPTION = "Trim superfluous PC-relative relocations";

	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		int i = 0;
		while (i < args.length) {
			File out = new File(args[i++]);
			AddressSet set = new AddressSet();
			while (i < args.length && !args[i].equals("--")) {
				String[] range = args[i++].split("-");
				set.add(toAddr(range[0]), toAddr(range[1]));
			}
			i++;
			export(out, set);
		}
	}

	private void export(File out, AddressSet set) throws Exception {
		AutoAnalysisManager aam = AutoAnalysisManager.getAnalysisManager(currentProgram);
		aam.scheduleOneTimeAnalysis(aam.getAnalyzer("Relocation table synthesizer"), set);
		aam.waitForAnalysis(null, monitor);

		Exporter exporter = null;
		for (Exporter e : ClassSearcher.getInstances(Exporter.class)) {
			if (e.getName().equals(EXPORTER)) {
				exporter = e;
			}
		}
		if (exporter == null) {
			throw new RuntimeException("Exporter not found: " + EXPORTER + " (is the delinker installed?)");
		}

		List<Option> options = new ArrayList<>(exporter.getOptions(() -> currentProgram));
		for (int i = 0; i < options.size(); i++) {
			Option o = options.get(i);
			if (o.getName().equals(TRIM_OPTION)) {
				options.set(i, new Option(o.getGroup(), o.getName(), false));
			}
		}
		exporter.setOptions(options);
		if (!exporter.export(out, currentProgram, set, monitor)) {
			throw new RuntimeException("Failed to export " + out);
		}
		println("Exported " + out);
	}
}
