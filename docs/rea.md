# Using rea

[rea](https://github.com/morluto/rea) is an MCP server and CLI that gives an agent (Claude Code, Codex, Cursor and others) access to a disassembler. In this project it is the analysis front-end: it opens `Tornado.exe` in Ghidra and returns pseudocode, assembly, strings, references and types, with the evidence behind each answer. rea does not build code or measure matching; that is the job of `configure.py`, ninja and objdiff.

## Setup

Requirements: Node.js 22.19+ (or 24.11+), a JDK and Ghidra. On Linux, rea also needs a procps-compatible `ps`.

```sh
npx rea-agents setup          # registers the MCP server with your agent
rea inspect "$PWD/orig/Tornado.exe" --provider ghidra --json
```

`Tornado.exe` is large (2.4 MB of code), so raise the Ghidra startup deadline if analysis times out, e.g. `REA_GHIDRA_STARTUP_TIMEOUT_MS=600000`.

## Workflow for one function

1. Ask the agent to inspect the function at an address: pseudocode, assembly, callers and callees, strings it references.
2. Write C++ for it in the unit's source file, using names and types from `include/`.
3. `ninja` to rebuild, then check the function in objdiff.
4. Iterate until objdiff reports 100%. When names or struct layouts are learned, add them to `config/symbols.txt` and `include/`.

rea's `compare_functions` tool compares pseudocode, assembly and control flow between two analyses. It is handy for spotting behavioural differences, but objdiff's byte-level match is the standard for this project.
