// Print the references from each given address.
// Usage: -postScript ProbeRefs.java <address> [...]
//@category TornadoJockey

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.symbol.Reference;

public class ProbeRefs extends HeadlessScript {
	@Override
	public void run() throws Exception {
		for (String arg : getScriptArgs()) {
			Address a = toAddr(arg);
			Reference[] refs = getReferencesFrom(a);
			StringBuilder line = new StringBuilder(arg + " refs=" + refs.length);
			for (Reference r : refs) {
				line.append("  ").append(r.getReferenceType()).append("->").append(r.getToAddress());
			}
			println(line.toString());
		}
	}
}
