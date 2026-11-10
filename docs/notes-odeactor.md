# Tongas2/ODEACTOR notes

## The ODE actor command handlers at 0x0054b670 / 0x0054b730 / 0x0054b7f0

Three 182-byte command handlers, one shape:

```text
sub esp, 0x10
zeros a V3 (three dwords) plus one ctor temporary
GetToken(1) on call->tokens (inline TOKENS::GetToken)
sscanf(text, "%f %f %f", &v.x, &v.y, &v.z)
if (m_body) dBodyAdd<Torque|RelForce|Force>(m_body, v.x, v.y, v.z)
return SRESULT(true)
```

| address | body call |
|---|---|
| 0x0054b670 | `_dBodyAddTorque` |
| 0x0054b730 | `_dBodyAddRelForce` |
| 0x0054b7f0 | `_dBodyAddForce` |

`TOKENS`/`STRING` already have the layout the exe uses for the inlined
`GetToken`: `m_string` starts at +0 (a `STRING`; its `m_buf` at +4), `m_count`
at +0x18 and `m_offsets` at +0x1c (`m_offsets[1]` is read at +0x20). No header
change is needed.

### Residue

Compiling the natural source gives 180 bytes instead of 182 and differs from
the exe by one saved register: the retail code saves `edi` and keeps `this`
there (`push edi; mov edi, ecx`), using `esi` for the `SRESULT` return buffer
at the end; VC7.1 in this unit keeps `this` in `esi` and reuses it for the
return buffer, so every stack offset in the function shifts by four and the
`dBodyAddTorque` field loads are scheduled after the body test instead of
before it. The instruction and call structure is otherwise identical
(objdiff 84.6% because of the resynchronised offsets).

This is the register-colour class in `docs/matching-levers.md`: compiling the
same source in a scratch translation unit that contains only these three
methods still gives 180 bytes and `push esi; mov esi, ecx` (checked with
`build/odetest.cpp`), so a unit split does not change it. The exe's `push edi`
choice is compiler state, not a different statement shape.
