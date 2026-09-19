# Source and build ownership map

Use this page when a function appears in more than one source file or when its
VC7 comparison owner differs from its playable-build owner.  It is a routing
map, not an exactness ledger.  `config/match-units.toml` and
`config/matches.csv` remain authoritative for accepted comparisons.

## Build selectors

| Question | Authoritative selector |
| --- | --- |
| What enters the modern playable executable? | `TH08_GAME_SOURCES` in `CMakeLists.txt` |
| What enters the native VC7 executable? | `cxx_sources`, `production_ecl_sources`, and `pbg_sources` in `scripts/configure.py` |
| What is compiled only for exact comparison? | `re_probe_sources` and the explicit `build/probes/*.obj` rules in `scripts/configure.py` |
| Which object and symbol does a strict comparison use? | `config/match-units.toml` |
| Which comparisons are accepted? | `config/matches.csv` |

A `*Probe.cpp` file is never a portable implementation.  Change the production
owner first, synchronize duplicated probe behavior when necessary, and replay
the configured probe unit.  Do not add probes to `TH08_GAME_SOURCES` or the
native link merely to make a comparison convenient.

## Source families

| Subsystem | Production owner | Comparison or shared source | Validation entry point |
| --- | --- | --- | --- |
| ECL dispatcher | `EclRun.cpp` | `EclRunLow.inl` owns opcodes 1–92; `EclRunHigh.inl` owns 93–184 and the shared tail. Both are lexically included inside `RunEcl`. | `build/EclRun.obj`; `compare-function.py ecl-manager-run-ecl` |
| ECL operands and helpers | `EclOperandsInt.cpp`, `EclOperandsFloat.cpp`, `EclHelpers.cpp`, `EclExIns.cpp`, `EnemyTimeline.cpp`, `EnemyManagerUpdate.cpp` | These use the generic non-PCH VC7 profile in `production_ecl_sources`. | Build the smallest object, then replay its units. |
| ECL-facing dependencies | `EclDependencies.cpp` | Physical owner for target-local spell, GUI, shot, and Enemy routines called by ECL; logical class names do not imply another TU. | `build/EclDependencies.obj` and its accepted units. |
| ECL manager | `EclManager.cpp` | Production is PCH-backed; `build/probes/EclManager.obj` recompiles the same source with the generic profile for selected comparisons. | Build/replay both owners when shared behavior changes. |
| Game setup and score | `GameManager.cpp` | `GameManagerSetup.cpp` and `GameManagerScoreProbe.cpp` are exact-only duplicate owners selected with the GameManager profile. | Production `build/GameManager.obj` plus the affected probe object. |
| Player options | `Player.cpp` | `PlayerOptionProbe.cpp` is an exact-only duplicate owner selected with the Player profile. | Production `build/Player.obj` plus `build/probes/PlayerOptionProbe.obj`. |
| Pause/retry menus | `AsciiManager.cpp` | `StageMenuProbe.cpp` contains compact exact-only layouts and draw bodies. | Production `build/AsciiManager.obj` plus `build/probes/StageMenuProbe.obj`. |
| Title/replay menus | `TitleScreen.cpp` | `TitleReplayMenuProbe.cpp` is exact-only. `TitleCompletionStatus.inl`, `TitleFormatSpellCardInfo.inl`, `TitleFullWidthDigits.inl`, and `TitleUnlockLastWords.inl` are shared fragments; `TitleSpellCardData.inl` is production data. | Production `build/TitleScreen.obj` plus the affected title probe units. |
| Effects | `EffectManager.cpp` | Public `Effect` and `EffectManager` layouts remain in `EclManager.hpp`; there is no separate probe owner. | `build/EffectManager.obj`; see `EFFECT_STORAGE.md`. |
| Replay | `ReplayManager.cpp` | Public serialized/runtime contracts are in `ReplayManager.hpp`. | `build/ReplayManager.obj` and the relevant replay unit. |
| Screen effects | `ScreenEffect.cpp` | Variant parameter contract is beside `RegisterChain` in `ScreenEffect.hpp`. | `build/ScreenEffect.obj` and all accepted units in it. |
| ANM | `AnmManager.cpp` | Layout, opcodes, manager file slots, and VM state live in `AnmManager.hpp`. | `build/AnmManager.obj`; see `ANM_RESOURCE_INDEX.md`. |

## Reading `EclRun`

`EclRun.cpp` first includes both `.inl` files in declarations-only mode.  It
then includes their handler bodies inside `EclManager::RunEcl` and inside the
opcode `switch`.  This preserves one VC7 stack frame, lexical handler order,
and cross-handler labels.

The included bodies intentionally see `enemy`, `instruction`,
`activeChildContext`, `context`, and `ctx` from the outer function.  Control can
also reach outer labels such as `low_advance_instruction`,
`low_redispatch_instruction`, and `low_select_next_context`; the high body owns
`enter_subroutine`.  Treat those names as a private include protocol.  Moving a
handler into an ordinary function is a code-generation change, not a cosmetic
cleanup.

## Validation recipes

Use a single build job.  A focused example is:

```bash
python3 scripts/build.py build/EclRun.obj -j 1
python3 scripts/compare-function.py ecl-manager-run-ecl
```

For a shared header, PCH, layout, compiler-profile, or object-partition change,
finish with the cold aggregate gate required by `RE_WORKFLOW.md`:

```bash
python3 scripts/analysis/verify-exact-units.py --all
python3 scripts/ci.py
git diff --check
```
