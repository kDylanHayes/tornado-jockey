// Delink each unit to a COFF object (a slimmed-down DelinkProgram.java from
// ghidra-delinker-extension that reports why an export failed and carries
// on with the remaining units).
// Usage: -postScript DelinkUnits.java <range[,range...]> <out.obj> ...
//@category TornadoJockey

import java.io.File;
import java.util.List;

import ghidra.app.plugin.core.analysis.AutoAnalysisManager;
import ghidra.app.services.Analyzer;
import ghidra.app.util.exporter.Exporter;
import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.AddressSet;
import ghidra.util.classfinder.ClassSearcher;

public class DelinkUnits extends HeadlessScript {
	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		Exporter exporter = ClassSearcher.getInstances(Exporter.class)
				.stream()
				.filter(e -> e.getName().equals("COFF relocatable object"))
				.findFirst()
				.get();
		AutoAnalysisManager aam = AutoAnalysisManager.getAnalysisManager(currentProgram);
		Analyzer synthesizer = aam.getAnalyzer("Relocation table synthesizer");

		for (int i = 0; i + 1 < args.length; i += 2) {
			AddressSet set = new AddressSet();
			for (String range : args[i].split(",")) {
				String[] ends = range.split("-");
				set.addRange(toAddr(ends[0]), toAddr(ends[1]));
			}
			File file = new File(args[i + 1]);
			aam.scheduleOneTimeAnalysis(synthesizer, set);
			aam.waitForAnalysis(null, getMonitor());
			exporter.setOptions(exporter.getOptions(() -> currentProgram));
			if (!exporter.export(file, currentProgram, set, monitor)) {
				println("DELINK FAILED " + file.getName() + ": " +
					exporter.getMessageLog().toString().strip().replace('\n', ' '));
			}
		}
	}
}
