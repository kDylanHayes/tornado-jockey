// Apply names from config/symbols.txt to the program, so delinked objects
// carry the same symbol names as our compiled objects.
// Usage: analyzeHeadless <proj> <name> -process Tornado.exe -noanalysis \
//          -scriptPath tools/ghidra -postScript ApplySymbols.java config/symbols.txt
//@category TornadoJockey

import java.nio.file.Files;
import java.nio.file.Paths;

import ghidra.app.util.headless.HeadlessScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.util.exception.DuplicateNameException;

public class ApplySymbols extends HeadlessScript {
	@Override
	public void run() throws Exception {
		// A function that is only a jump to another one (jpeg_destroy_compress:
		// jmp _jpeg_destroy) is a thunk to Ghidra and borrows its target's
		// name, which gives a delinked object two symbols of that name. Make
		// such functions ordinary; thunks to imports keep their DLL names.
		for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
			Function target = f.getThunkedFunction(false);
			if (target != null && !target.isExternal()) {
				f.setThunkedFunction(null);
			}
		}

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
			boolean isFunction = parts.length > 2 && parts[2].equals("function");
			Function f = getFunctionAt(addr);
			if (isFunction && parts.length > 3) {
				f = setExtent(f, addr, Long.decode(parts[3]));
			}
			if (isFunction && f == null) {
				f = splitAt(addr);
			}
			if (isFunction && getInstructionAt(addr) == null) {
				// Only reached through a pointer, so analysis never
				// disassembled it (png error_handler); the delinker
				// needs instructions to find its relocations.
				disassemble(addr);
			}
			try {
				if (f != null) {
					f.setName(name, SourceType.USER_DEFINED);
				}
				else {
					createLabel(addr, name, true, SourceType.USER_DEFINED);
				}
				applied++;
			}
			catch (DuplicateNameException e) {
				// Analysis already put this name here as a secondary label
				// (FID does for _fprintf); promote it.
				Symbol existing = getSymbol(name, null);
				if (existing != null && existing.getAddress().equals(addr)) {
					existing.setPrimary();
					applied++;
				}
				else {
					printerr("Could not name " + addr + " " + name + ": " + e.getMessage());
				}
			}
		}
		println("Applied " + applied + " symbols");
	}

	// A function starts at addr, but analysis made it part of the function
	// before (a 6-byte "return string" after its neighbour, say). End that
	// one at addr and create the function.
	private Function splitAt(Address addr) throws Exception {
		Function outer = getFunctionContaining(addr);
		if (outer != null && !outer.getEntryPoint().equals(addr)) {
			AddressSet body = new AddressSet(outer.getBody());
			body.deleteRange(addr, body.getMaxAddress());
			try {
				outer.setBody(body);
			}
			catch (Exception e) {
				printerr("Could not end " + outer.getName() + " at " + addr + ": " + e.getMessage());
				return null;
			}
		}
		Function f = createFunction(addr, null);
		if (f == null) {
			printerr("Could not create a function at " + addr);
		}
		return f;
	}

	// Make the function at addr span exactly size bytes. Analysis sometimes
	// splits a function in two (e.g. after a call it thinks never returns);
	// any function starting inside the range is removed.
	private Function setExtent(Function f, Address addr, long size) throws Exception {
		Address end = addr.add(size - 1);
		FunctionManager fm = currentProgram.getFunctionManager();
		for (Function inner : fm.getFunctions(new AddressSet(addr.add(1), end), true)) {
			if (inner.getEntryPoint().compareTo(addr) > 0) {
				fm.removeFunction(inner.getEntryPoint());
			}
		}
		if (f == null) {
			f = createFunction(addr, null);
			if (f == null) {
				return null;
			}
		}
		AddressSet full = new AddressSet(addr, end);
		if (!f.getBody().equals(full)) {
			try {
				f.setBody(full);
			}
			catch (Exception e) {
				printerr("Could not resize " + addr + ": " + e.getMessage());
			}
		}
		// Code after a call that never returns (_CxxThrowException) is a
		// catch block only the EH tables point to; analysis never reached
		// it, but the delinker needs its instructions for relocations.
		// (Not every undefined byte: jump tables are undefined too.)
		Listing listing = currentProgram.getListing();
		boolean more = true;
		while (more) {
			more = false;
			for (Instruction ins : listing.getInstructions(new AddressSet(addr, end), true)) {
				Address next = ins.getMaxAddress().add(1);
				if (!ins.getFlowType().isCall() || next.compareTo(end) > 0 ||
					listing.getInstructionContaining(next) != null ||
					listing.getDefinedDataContaining(next) != null ||
					(currentProgram.getMemory().getByte(next) & 0xff) == 0xcc) {
					continue;
				}
				disassemble(next);
				more = true;
				break;
			}
		}
		return f;
	}
}
