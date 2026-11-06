# TEXTFILE unit (0x00528610-0x0052d3ff)

The unit is named after the `TEXTFILE - file does not exist` string at
`0x00528610`, but it holds more than the tokenizer. The carved object also
contains the particle/animation classes and the VC71 `vector` instantiations
they use.

At the moment the unit is 15.36% exact / 21.18% fuzzy, 3014/19625 bytes,
21/40 paired functions.

## Identified classes

- `TEXTREADER` (loader `0x00528610`, string reader `0x00528770`): a wrapper
  with a `STREAM *` at `+0`. `Load(const std::string&)` copies the name into
  a 512-byte buffer and drives stream virtuals at `+4` (open) and `+0xc`
  (is-open). `ReadString()` calls the virtual at `+0x20` and returns the C
  string as a `std::string`.
- `TEXTFILE` (constructor `0x00528b00`, `GetToken` `0x00528aa0`): the
  116-byte constructor zeroes six `std::vector` members and three pointers,
  then returns `this`. Layout: vectors at `+0x00`, `+0x10`, `+0x20`,
  `+0x4c`, `+0x70`, `+0x84`; pointers at `+0x30`, `+0x34`, `+0x80`.
  `GetToken(void*, int)` initializes the tokenizer at `+0x84` and forwards
  to the reader object at `m_030->m_1c` (or `m_034->m_1c`).
- `ANIMTRACK` (frame interpolation `0x52a8d0`): the cached `V3` is at
  `+0x10` and a `std::vector<V3>` begins at `+0xa4` (begin `+0xa8`, end
  `+0xac`); elements are 0xc bytes. `GetPoint` clamps two frame indices,
  interpolates by a float and copies the cached position to the output.
- `PARTICLE` (constructor `0x0052ce60`, teardown `0x0052cef0`): 0x84-byte
  class with two `V3` members at `+0x04`/`+0x10`, five vectors at `+0x1c`,
  `+0x34`, `+0x48`, `+0x58`, `+0x74`, the reference object at `+0x44`, two
  ints at `+0x2c`/`+0x30`, and the float `1.0` at `+0x68`.
- `PARTICLEB` (constructor `0x0052b3a0`, member teardown `0x0052ae10`):
  0x5c bytes of prefix, two `std::string` members at `+0x5c`/`+0x78`, and
  two `std::vector<int>` members at `+0x94`/`+0xa4`.
- `BONEDMODELDX8PARTICLEDESC` is only referenced, not defined here; its
  constructor lives in `BONEDMODELDX8SD`.

## Exact functions

The first two batches are the VC71 `std::vector` bodies:

- `0x0052aeb0` `vector<int>::_Buy`
- `0x0052af00` `vector<int>::_Insert_n`
- `0x0052b0e0` `vector<int>::operator=`
- `0x0052b230` / `0x0052b270` forward/backward copy loops
- `0x0052b370` fill loop
- `0x0052cc00` `vector<vector<int>>::push_back`
- `0x0052cc70` `vector<int>::push_back`
- `0x0052ccc0` `vector<vector<int>>::_Uninitialized_copy`
- `0x0052cdd0` `vector<vector<int>>::_Uninitialized_fill_n`
- `0x0052d080` `vector<vector<int>>::_Ufill`
- `0x0052d0b0` `vector<vector<int>>::_Insert_n`

`_Uninit_copy`, `_Uninit_fill_n` and `_Insert_n` blend their catch funclets
into the parent Ghidra function, so their `config/symbols.txt` sizes cover
the full `0x8e`, `0x8c` and `0x298` bytes; the real STL instantiations then
match byte-for-byte.

The particle side contributes:

- `0x0052a120` / `0x0052a130` reference release wrappers
- `0x0052aa20` key scan (`PARTICLE::FindKey`)
- `0x0052ade0` vector teardown helper
- `0x0052ae10` `PARTICLEB::DestroyMembers`
- `0x0052b3a0` `PARTICLEB` string constructor
- `0x0052ce60` `PARTICLE` `(int,int)` constructor
- `0x00528b00` `TEXTFILE` default constructor
- `0x00528770` `TEXTREADER::ReadString` (returns the stream's C string as a
  `std::string`; the EH handler label is mapped in `units.json`)

## Reconstructed but not exact

- `0x00528aa0` `TEXTFILE::GetToken` is 94.87%: one `mov ecx, eax` is
  scheduled one instruction before the argument pushes instead of after.
- `0x00528610` `TEXTREADER::Load` is 74.85% and exactly 310 bytes against
  the 344-byte target. The target saves ESI earlier and builds the local
  filename string in a different stack slot; the sequence of stream calls,
  `FileExists` and `ContentError` is otherwise reproduced.
- `0x0052a8d0` `ANIMTRACK::GetPoint` is 87.79%; the 320-byte reconstruction
  has the same arm as the 324-byte target but the target keeps the first
  frame index in ESI from the function entry.
- `0x0052cef0` `PARTICLE::Destroy` is 84.67% and exactly 393 bytes. The
  target's element loop reloads the track pointer and has an extra null
  check; the cleanup order and duplicate `m_048.clear()` are reproduced.
- `0x0052b2b0` `vector<int>` copy constructor is 98.33%; the target's
  catch funclet is inside the 183-byte function while this build emits a
  separate `__ehhandler` section (184 bytes).
- `0x0052ad70` `vector<int>::_Xlen` is 95.43% for the same reason: the
  function body is 105 bytes but the COFF symbol carries one padding byte.

## Remaining blockers

- `0x00528820` splits a line into a custom circular string list. The
  container has a pointer array at `+4`, capacity at `+8`, circular offset
  at `+0xc` and count at `+0x10`; its grow routine is `0x527c20` and each
  string is a separately allocated 0x1c-byte object. Reproducing the inline
  add path is needed before the splitter can be transcribed.
- `0x0052b430` (6084 bytes) is not a vector helper: it is the particle draw
  routine. It fetches the descriptor's `+0x38` surface, calls the D3D vtable
  at offsets `0x94`/`0x130`/`0x14c`/`0x154`/`0xfc`, and reads many settings
  through `SETTINGSTABLE::GetInt`.
- The animation/bone math at `0x0052a140`, `0x0052a2c0`, `0x0052aab0` and
  `0x0052ab80` still needs the matrix/bone layout. `0x0052a2c0` is a
  1549-byte copy/transform routine; `0x0052a140` builds a matrix from two
  frames and calls `0x00463dd5`/`0x00463397`.
- `0x00528e60` (4757 bytes) is the text model loader; it needs the model
  class and the `BONEDMODELDX8PARTICLEDESC` constructor interface.
