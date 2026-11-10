# Matching levers (MSVC 7.1)

A checklist for a function that plateaus below 100%. It is adapted from the
[matching-decompilation skill set](https://github.com/lemiur/Game-Decompilation-Skill-For-Claude)
and narrowed to what has been proven on this project's compiler (`cl`
13.10.3077) and tools. Use one lever at a time and compile the real unit; a
fuzzy score alone is not a verdict.

## Classify the wall first

Do not call a wall "register allocation" while an earlier class still differs.

| class | signal | what to do |
|---|---|---|
| referent | masked bytes identical but score below 100 | fix symbol, constant or relocation identity in `config/symbols.txt`; this is labelling, not codegen |
| inline / call set | the set or count of out-of-line calls differs | finish the caller body, restore the inline helper or macro boundary ([helper recovery](#inline-helpers-and-macros)) |
| control flow | block, branch or return counts differ | change the source construct: loop form, early returns, `goto`, short-circuit collapse |
| register / schedule | calls, branches and constants agree; only registers, spills or one instruction slot differ | the levers below, then stop: this class is compiler state |

## 1. Recover source before steering the compiler

- `git log -S'<function>' -- src include` for a better earlier revision.
- Read the pseudocode (`py tools\decompile.py <unit>`), then read the retail
  assembly; the decompiler's variable merges are hints, not source.
- Use sibling functions in the same class and unit, and the strings the binary
  carries (`"CLASS::Method> ..."`), for ownership, argument widths and names.
- Check unit, class, receiver and calling-convention identity before treating
  an operand difference as scheduling.

## 2. Types, layout and aggregates

- Propogate parameter widths through the family: `bool`/`char`/`unsigned char`,
  signed against unsigned, 16-bit stores, real SDK types.
- Restore aggregates only from whole-object evidence: `V2`, `V3`, `V3I`,
  `MATRIX`, `RESHANDLE`, `ANG`, `FIXEDSTRING<N>`, `WIDESTRING<N>`.
- A wrong type changes more than its own load: it changes the temporary home,
  the evaluation order and the register the value lands in.
- Placeholder classes may name members by offset (`m_1b9c`), but prefer a real
  member name as soon as the role is known.

## 3. Inline helpers and macros

- Search for an authored helper before writing the expansion by hand:
  accessors, `operator[]`, getters/setters, `GetPosition`, `GetScale`,
  `SetName`, `SafeSprintf`, `memcpy`, `free`, `operator new`.
- A helper expands at some sites and is called at others; check the call set
  and the object's COMDATs rather than assuming.
- Hidden return buffers matter: `obj->GetPosition()` materialises a temporary
  whose address is passed. Passing such a temporary straight into another call
  preserves one home where a named local forces two.
- A member-call form can fix evaluation order that a free operator cannot:
  `V3 delta = temp.DiffFrom(other)` evaluates the argument first and computes
  `other - temp` (this is how `G2CHAIN::B` reached 100%).

## 4. Locals, scopes, lifetime and creation order

- One local per meaning, named for it. A decompiler's merged variable is often
  two variables.
- Declaration order and *first-use* order are separate; creation order can
  decide register preference, spill-slot reuse and the frame size.
- Declaration initialiser against a later store; initialisation in a `for`
  header; deliberately uninitialised locals — each emits differently.
- Sibling block locals may share one home; function-scope locals cannot.
- Compare the frame size and every address-taken `lea` before calling a stack
  difference a compiler quirk. A frame difference is a real local, an unnamed
  temporary, a reused parameter home or a spill.
- Hoisting the scalar locals of a loop (and zeroing a `V2` in place) was what
  put `G2CHAIN::B`'s buffers on the target's slots.

## 5. Statement and expression shape

- Test one expression against sequenced assignments, and a direct member store
  against a value reused through setters.
- Put each computation in the statement retail emits it in: naming an outer
  call argument, moving `+ K` into an initialiser, or grouping constructor
  arguments changes scheduling.
- Test the alternatives: ternary against split `if`; short-circuit against a
  materialised bool; compound update against copy and write-back.
- Preserve operand order for non-commutative operations. The compiler may
  canonicalise commutative ones; do not infer source order from those.
- `fsubr` against `fsub` is an evaluation-order readout: a named delta local
  (`float dt = g_pTimer->m_dt; m_timer -= dt;`) produced retail's
  `fld dt; fsubr timer` in `G2CHAIN::B`.
- Literal arguments create their own temporaries: `SetActivation(1)` matched
  where a named `int one` did not.
- Era idioms: `memcpy`, `memset`, `strlen`, `free`, `operator new`, `abs`.

## 6. Guards, exits and block placement

- Count calls, conditional branches, unconditional jumps and returns before
  calling a wall register allocation. Match destinations too.
- Test separate early returns, `goto fail`, and total `||`/`&&` collapse; they
  are different tail-merging regimes.
- Check guard polarity and whether the success body is nested under the guard
  or the failure paths return early.
- Destructors and EH arms are special: unwind targets and the EH-state count
  prove the authored cleanup layer.

## 7. Loops and cursor ownership

- Test `while`, guarded `do/while`, `for`, infinite-loop-plus-`break`, and
  duplicated `continue` paths against retail's entry and latch.
- Reproduce update order, including comma increments (`i++, p += 0xc` fixed
  the last two instructions of `G2ODESWAP::Swap`).
- Decide who owns the cursor: the parameter, an interior pointer, an indexed
  access or a member-biased array pointer.
- An entry trampoline is a readout of value ownership or pressure, not a
  request to respell the loop.

## 8. Allocation, destruction and storage

- Prefer real `new`/`delete` and array forms when ownership supports them.
- Destructor chains, deleting destructors and vtable resets are compiler
  generated: reconstruct the class shape (base, virtuals, members) and let the
  compiler emit the thunk rather than writing it by hand.
- Guarded delete plus null store, direct member out-parameter and local copy,
  `memset` against a store loop: test them in that order.

## 9. Data, constants and referents

- Audit ordered relocations, not just masked instructions: a wrong callee,
  pooled literal, member, global or FP constant slot can be the whole residue.
- Name the data your object references in `config/symbols.txt` (`__real@...`,
  `??_C@...`, factory ids, counters). `G2CHAIN::B` gained a point when its
  strings, margins, factory ids and counter were named.
- Use the era's literal forms: `1.0f / RAND_MAX` (not `1.0f / 32768.0f`) in
  the chain pickers, bare strings under `/GF` string pooling.
- Constants that fold to a single instruction need a symbol only when a
  relocation is present.

## 10. Stop signals

- A pure destination-register colour with identical IL is cursor phase. Do not
  manufacture a local to request a register.
- A single scheduler slot (one `fcos` or `fmul`) is compiler state; bank the
  source and record the residue.
- A folded addressing mode that retail splits (`lea` + `mov`) is an unresolved
  code-shape or helper boundary, not a reason to distort the expression.
- Record bounded residue in the unit notes or the commit message; do not keep
  probes or dummy locals.

## Current known walls

| function | score | classification |
|---|---:|---|
| `G2CHAIN::C` / `D` | 96.72 / 96.11 | register colour: `this` in `ebp` vs retail's `edi`; instruction stream identical |
| `MATRIX::RotateY` | 90.46 | schedule: `fcos` against one zero store |
| `G2RULESOBJ::Update` | 95.92 | schedule: `fmul` against one `lea` |
| `SKINNEDMESH` `Get14`/`Get4` | 68 | code shape: retail uses `lea` + `mov`, the compiler folds to `mov [eax+ecx*4]` |
| `G2CHAIN` constructor | 87.74 | schedule: interleaving around the wide-string pushes |
