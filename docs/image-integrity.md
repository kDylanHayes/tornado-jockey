# Opaque bytes in the ISO-extracted image

`orig/Tornado.exe` (SHA-1 `20bf7200…`, the installer's `Tornado-WT.exe`
payload) is not a clean copy of the retail binary: at several places its
`.text` and `.rdata` contain short runs of opaque bytes that overwrite the
real code or data. They look like random bytes in the middle of a function or
a string table, not like anything the compiler or the linker writes.

They cannot be decompiled. Do **not** put those bytes in the source, widen a
data table to cover them, or otherwise fabricate data to make objdiff report
100%: the unit stays partial and the address is listed here.

Every entry below was confirmed by comparing the bytes at the address with
what our compiled object produces for the same symbol.

| VA | Unit | Symbol / item | Patched bytes |
|---|---|---|---|
| `0x004c6790` | `Tongas2/GUISLIDERCONTROL` | `?SetValueAt@GUISLIDER@@QAEXMM@Z` | 16 bytes at the function's prologue; code resumes at +16 |
| `0x004c8ec0` | `Tongas2/GUIVISUALCONTROL` | `?SetVisualOpacity@GUIVISUAL@@UAEXABHABM@Z` | 6 bytes at the function's prologue; code resumes at +6 |
| `0x004cb5da` | `Tongas2/GUITEXTCONTROL` | `?Execute@GUITEXT@@UAE?AVSRESULT@@PAUCMDENTRY@@PAPAVTOKENS@@@Z` (+74) | 16 bytes mid-function; code resumes at +90 |
| `0x0048e33d` | `Tongas2/GUI` | `??1GUI@@UAE@XZ` (+77) | 16 bytes mid-function; code resumes at +93 |
| `0x004d2b48` | `Tongas2/OCTREE` | 16 bytes inside a function | reported by the octree worker |
| `0x00671060`, `0x00671070` | `Tongas2/SHADER` | string prefixes in `.rdata` | reported by the shader worker |
| `0x00667150`, `0x0066c0d8` | `Tongas2/CMDMGR`, `Tongas2/RESOURCE` | `.rdata` string/table bytes | reported by those units' workers |
| `0x00673827` | `Tongas2/BEHAVIORFADERESLOOP` | 16 bytes inside its `_s_TypeNames` copy | reported by the behavior worker |

`SetValueAt` and `SetVisualOpacity` therefore never reach 100%, and
`GUISLIDERCONTROL` / `GUIVISUALCONTROL` stay partial. Everything else in
those units can still match.
