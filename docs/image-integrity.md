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
| `0x004d0426` | `Tongas2/GUIBEHAVIORS` | `?Update@ACCELERATION@@QAE_NM@Z` (+0x156) | 16 bytes mid-function (Ghidra reports overlapping instructions there) |
| `0x0049f540` | `Tongas2/GUIGROUPCONTROL` | `?MouseOverCommand@GUIGROUP@@UAE?AVSRESULT@@PAUCMDCALL@@@Z` (+112) | 18 bytes mid-function; code resumes at +130 |
| `0x00493187` | `Tongas2/GUICONTROL` | `?MoveRel@GUICONTROL@@UAE?AVSRESULT@@PAUCMDCALL@@@Z` (+23) | 16 bytes in the function body; code resumes at +39 |
| `0x00615a5a` | `Tongas2/GUIGENERICCONTROL` | `_$E3` (+522), the property-table initializer | 21 bytes mid-function; code resumes at +543 |

Data is patched too, in the GUI tables the initializers fill:

| VA | Unit | Item | Patched bytes |
|---|---|---|---|
| `0x006c1290` | `Tongas2/GUIVISUALCONTROL` | `GUIVISUAL::s_commands` +48 | 16 bytes |
| `0x006ba290` | `Tongas2/GUICONTROL` | inside `GUICONTROL::s_commands` +640 and `s_properties` +384 (the two tables' ranges overlap in our layout) | 16 bytes |
| `0x004d2b48` | `Tongas2/OCTREE` | 16 bytes inside a function | reported by the octree worker |
| `0x00671060`, `0x00671070` | `Tongas2/SHADER` | string prefixes in `.rdata` | reported by the shader worker |
| `0x00667150`, `0x0066c0d8` | `Tongas2/CMDMGR`, `Tongas2/RESOURCE` | `.rdata` string/table bytes | reported by those units' workers |
| `0x00673827` | `Tongas2/BEHAVIORFADERESLOOP` | 16 bytes inside its `_s_TypeNames` copy | reported by the behavior worker |

`SetValueAt`, `SetVisualOpacity`, `GUITEXT::Execute`, `GUI::~GUI`,
`ACCELERATION::Update`, `GUIGROUP::MouseOverCommand`, `GUICONTROL::MoveRel`,
`GUIGENERICCONTROL`'s `_$E3` and the two GUI tables above therefore never
reach 100%, and the units that own them (`GUISLIDERCONTROL`,
`GUIVISUALCONTROL`, `GUITEXTCONTROL`, `GUI`, `GUIBEHAVIORS`,
`GUIGROUPCONTROL`, `GUICONTROL`, `GUIGENERICCONTROL`) stay partial.
Everything else in those units can still match.
