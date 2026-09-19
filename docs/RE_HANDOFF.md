# Current reconstruction handoff

This file is deliberately short and replaceable. Historical checkpoints are
preserved in [RE_HANDOFF_HISTORY.md](RE_HANDOFF_HISTORY.md); live counts come
from the ledgers rather than either prose file.

## Current status

The target is the original Japanese TH08 1.00d executable, size 840,704 bytes,
with SHA-256
`330fbdbf58a710829d65277b4f312cfbb38d5448b3df523e79350b879213d924`.

Authored source covers **1,107 / 1,107** authored functions and the accepted
exact ledger covers **1,106 / 1,107**. The sole authored near match remains
`ReplayManager::PlaybackExtendedInputAndFps @ 0x004526C0`. Recompute these
figures with:

```bash
python3 scripts/analysis/report-reconstruction-status.py --summary
```

The native Windows i386 prerequisite is complete and merged. Its clean VC7
bugfix build was playtested through Final with normal score/graze behavior,
dialogue/background rendering, Stage 4 Reimu rendering, spell effects, and
replay save. The exact-facing native `normal` build and playable `bugfix` build
remain separate by design. See `WINDOWS_I386_RUNTIME.md`, `OWNER_AUDIT.md`, and
`RUNTIME_ISSUES.md` for the reproducible evidence.

Portable 64-bit and Web follow-up work is maintained on their corresponding
branches/repositories; it does not replace the native VC7 prerequisite or the
strict target comparison.

## Completed maintainer-navigation batch

The 2026-09-19 batch makes repository knowledge easier to enter without
changing target behavior:

- `SOURCE_MAP.md` maps production owners, exact probes, shared includes, build
  selectors, and validation entry points;
- `SEMANTIC_INDEX.md` routes current subsystem/owner/evidence questions;
- `ANM_RESOURCE_INDEX.md` separates file-slot, script, sprite, and VM-index
  namespaces and records the remaining opcode evidence queue;
- `EFFECT_STORAGE.md` records manager ownership, pool ranges, sentinel
  behavior, and callback-local scratch-vector roles;
- source headers identify ECL/probe/Effect compile roles;
- Replay, Effect, and ScreenEffect declarations carry their ownership or
  tagged-parameter contracts;
- `EclRun` uses the named enemy-position operand selectors and remains exact;
- ANM opcode 83 and its VM field now name the player-bullet draw mode and
  `AnmManager::ExecuteScript @ 0x0045EA00` remains exact.

## Validation

Focused results already established in this batch:

- `EclManager::RunEcl @ 0x004184B0`: exact, 26,638 authored bytes and 27,398
  compared bytes;
- `AnmManager::ExecuteScript @ 0x0045EA00`: exact, 13,933 authored bytes and
  14,349 compared bytes.

The shared-header gate was run from a cold state with one build job and passed
**1,106 / 1,106** accepted units. The complete portable Linux i386 container
target compiled and linked with one job; `verify-modern-linux.sh` confirmed
ELF32/ET_EXEC/i386 and every fixed target-owned layout symbol. The normal VC7
production image linked successfully. `python3 scripts/ci.py`, documentation
link validation, and `git diff --check` all pass.

## Next bounded work

With this documentation/readability batch complete, select one evidence-backed
family at a time. The current ANM queue intentionally leaves opcodes 25, 31,
and 88 neutral until their complete TH08 consumer sets justify a shared-layout
rename. Whole-image/library work remains independent and should resume only for
a bounded link dependency.
