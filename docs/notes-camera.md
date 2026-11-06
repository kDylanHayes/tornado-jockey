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

## Sweeps that did not close the gap

- `MATRIX::View`: splitting the three translation terms into named `float`
  locals raises the byte match from 409 to 415 of 474, but no ordering of those
  locals (all six) is exact. Permuting the two multiplicands of each term in the
  inline `Cross` (64 forms, compiled in one mock TU) leaves the code at 415 too:
  the compiler canonicalises the commutative pairs. The residual is the
  interleaving of the cross-product loads with stores, not the expressions.
- `CAMERA::Update`: the whole difference is 21 bytes at `0xa0`-`0xb4`. Source
  forms tried: both operand orders of the sum, component-wise `V3`,
  component-wise `Set`, `+=`, named `V3`, `const V3 &` binding, and all six
  orders of the three `m_jitter` assignments. The best remains the current
  `m_jitter + m_position` at 419 of 439; the target stores `m_jitter.y` before
  loading the vtable and then adds position-first only for `x`.
- `CAMERALENS::UpdateView`: more than twenty shapes were compiled (named
  eye/at, one reused local, arrays, copy-init, `Set`, direct expressions). The
  target allocates the eye temporary at `[esp]` and the at temporary at
  `[esp+0xc]`; VC7.1 gives the first call argument the higher slot in every
  shape, so the same 8 displacement bytes stay swapped. Function length and
  every instruction are otherwise identical.
- `MATRIX::Determinant`: the 91-byte target keeps `_11`-`_14` on the x87 stack
  and interoperates them with `faddp st(4)`/`fxch st(4)`, which is atypical for
  this compiler (it looks hand-written). A naked `__asm` copy compiles to the
  exact bytes except the `call`: MSVC's inline assembler cannot spell the
  decorated `?Adjoint@MATRIX@@QAE?AU1@XZ` symbol, so the call cannot carry the
  target's relocation without a build-system change. Left as source.
