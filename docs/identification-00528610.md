# TEXTFILE unit (0x00528610-0x0052d3ff)

The unit is named after the `TEXTFILE - file does not exist` string at
`0x00528610`, but it holds more than the tokenizer. The carved object also
contains the particle/animation classes and the VC71 `vector` instantiations
they use.

At the moment the unit is 14.49% exact / 15.89% fuzzy, 2844/19625 bytes,
20/40 paired functions.

## Identified classes

- `TEXTFILE` (constructor `0x00528b00`, `GetToken` `0x00528aa0`): the
  116-byte constructor zeroes six `std::vector` members and three pointers,
  then returns `this`. Layout: vectors at `+0x00`, `+0x10`, `+0x20`,
  `+0x4c`, `+0x70`, `+0x84`; pointers at `+0x30`, `+0x34`, `+0x80`.
  `GetToken(void*, int)` initializes the tokenizer at `+0x84` and forwards
  to the reader object at `m_030->m_1c` (or `m_034->m_1c`).
- `PARTICLE` (constructor `0x0052ce60`): 0x84-byte class with two `V3`
  members at `+0x04`/`+0x10`, five vectors at `+0x1c`, `+0x34`, `+0x48`,
  `+0x58`, `+0x74`, the reference object at `+0x44`, two ints at
  `+0x2c`/`+0x30`, and the float `1.0` at `+0x68`.
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

## Reconstructed but not exact

- `0x00528aa0` `TEXTFILE::GetToken` is 94.87%: one `mov ecx, eax` is
  scheduled one instruction before the argument pushes instead of after.
- `0x0052b2b0` `vector<int>` copy constructor is 98.33%; the target's
  catch funclet is inside the 183-byte function while this build emits a
  separate `__ehhandler` section (184 bytes).
- `0x0052ad70` `vector<int>::_Xlen` is 95.43% for the same reason: the
  function body is 105 bytes but the COFF symbol carries one padding byte.

## Remaining blockers

- `0x00528610` and `0x00528770`/`0x00528820` operate on a file-wrapper class
  with a stream pointer at `+0`, not on the reconstructed `TEXTFILE`; the
  current `?Load@TEXTFILE` name is a placeholder. `0x00528610` marshals a
  `std::string` into a 512-byte stack buffer and drives a virtual stream.
- `0x0052cef0` is a hand-written `PARTICLE::Destroy`-style method (release
  `m_044`, delete the track vector's elements, clear the vectors in a
  duplicated order). A direct transcription is 80% but the EH frame and
  element loop differ.
- `0x0052b430` (6084 bytes) and `0x0052d0b0`'s caller have not been
  transcribed.
- The animation math at `0x0052a140`, `0x0052a2c0`, `0x0052a8d0`,
  `0x0052aab0`, `0x0052ab80` still needs the bone/frame class layout.
- `0x00528e60` (4757 bytes) is the text model loader; it needs the model
  class and the `BONEDMODELDX8PARTICLEDESC` constructor interface.
