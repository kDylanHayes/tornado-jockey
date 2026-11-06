# CAMERA matching notes

`Tongas2/CAMERA` is at 65.93% exact / 97.36% fuzzy, 50 of 57 functions.

## What matched

- `??0CAMERALENS@@QAE@XZ` matches by writing the direction and up vectors with
  `m_direction.Set(0.0f, 0.0f, 1.0f)` / `m_up.Set(0.0f, 1.0f, 0.0f)` instead of
  component stores. The target's copies and the 1.0f constant loads only line
  up in that form.
- `??0CAMERA@@QAE@XZ` matches the same way, replacing the six `m_up` /
  `m_direction` component stores with two `V3::Set` calls.
- `?RegisterCommands@CAMERA@@SAXXZ` is exact with the refreshed delinked target
  (the earlier 99.58% was a stale relocation name in the shared build/target).

## Remaining functions

| Function | Size | Exact | Shape |
|---|---|---|---|
| `MATRIX::Determinant` | 91 | 69.51% | 77 bytes here, 91 in the target |
| `CAMERA::ResetMotion` | 153 | 86.77% | schedule only |
| `CAMERA::ScreenToWorld` | 567 | 89.57% | schedule and temp slots |
| `MATRIX::View` | 474 | 92.15% | schedule only |
| `CAMERA::Reset` | 333 | 94.87% | watermarked, left alone |
| `CAMERA::Update` | 439 | 98.42% | one FPU operand order |
| `CAMERALENS::UpdateView` | 115 | 99.84% | 8 stack-displacement bytes |

The last five are the same instruction stream as the target with loads, stores
and stack temporaries scheduled differently. The unit was swept with `/G7`,
`/Oa` and `/Ob1` earlier (see `docs/toolchain.md`), so the residual order is a
source-shape problem, not a flag problem.

- `Determinant` keeps `_11`, `_12`, `_13`, `_14` on the x87 stack before the
  `Adjoint` result is copied to the local matrix; every expression order,
  copy-init form and named-local form tried compiles to the 77-byte lazy-load
  schedule. The target's term order (`_14`, `_13`, `_11`, `_12`) is otherwise
  what the current source has.
- `UpdateView` needs the eye temporary at `[esp]` and the at temporary at
  `[esp+0xc]`. VC7.1 gives the first call argument the higher slot in every
  form tried (two named locals, one reused local, copy-init, direct expression,
  `Set`), so the eight displacement bytes stay swapped.
- `ResetMotion`'s only differences are the hoisted `lea` for the `SetPosition`
  argument and the late `m_rotateTarget.flags` store; inlining the three `Stop*`
  bodies by hand or moving the flag assignment out of `StopRotate` does not
  change the schedule.
