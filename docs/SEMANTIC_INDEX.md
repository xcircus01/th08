# Current semantic index

This is the short, current route from a game concept to its declaration,
implementation, and evidence record.  It deliberately does not repeat the
chronological batch narratives in `SEMANTIC_HISTORY.md`.

| Subsystem | Current declaration / owner | Evidence and navigation | Boundary still worth remembering |
| --- | --- | --- | --- |
| ANM interpreter and rendering | `AnmManager.hpp`, `AnmManager.cpp` | `ANM_RESOURCE_INDEX.md`; search semantic history for `ANM projection` | Manager slots, file-local script IDs, sprite IDs, and VM array indices are different namespaces. |
| ECL interpreter | `EclManager.hpp`, `EclRun.cpp`, `EclRunLow.inl`, `EclRunHigh.inl` | `RUNECL_FUNCTION_EXACT_NOTES.md`; `SOURCE_MAP.md` | Included handler bodies share an outer lexical frame and labels. |
| ECL operands/dependencies | `EclOperands.hpp`, `EclOperands*.cpp`, `EclDependencies.cpp`, `EclHelpers.cpp`, `EclExIns.cpp` | Search semantic history for `ECL` and `Enemy ECL` | Physical TU ownership follows target emission, not the class named by a function. |
| Enemy and timeline | `EnemyManager.hpp`, `EnemyManager.cpp`, `EnemyManagerUpdate.cpp`, `EnemyTimeline.cpp` | Semantic entries for Enemy motion/control/update and manager orchestration | TH06/TH07 names remain corroboration until TH08 dataflow confirms them. |
| Effect pool and callbacks | `EclManager.hpp`, `EffectManager.cpp` | `EFFECT_STORAGE.md`; semantic entry `Effect factory` | `vector1`…`vector7` are callback-local scratch roles, not global meanings. |
| Replay | `ReplayManager.hpp`, `ReplayManager.cpp` | Semantic entries `Replay runtime` and `Replay file envelope` | `LoadReplayData` consumes its encoded input and returns a separately owned allocation. |
| Screen effects | `ScreenEffect.hpp`, `ScreenEffect.cpp` | Semantic entry `ScreenEffect lifecycle` | `RegisterChain` parameters are a tagged protocol; `duration` stores amplitude for shake envelope. |
| Player, shots, and bombs | `Player.hpp`, `Player.cpp`, `PlayerBomb.cpp` | `PLAYER_MATCHING.md`; semantic Player/PlayerShot/PlayerBomb entries | Exact-only option bodies also exist in `PlayerOptionProbe.cpp`. |
| Bullets and lasers | `BulletManager.hpp`, `BulletManager.cpp` | Search semantic history for `Bullet/Laser` and `laser` | Serialized instruction shapes and runtime pool objects must remain distinct. |
| GUI and dialogue | `Gui.hpp`, `Gui.cpp`, `AsciiManager*.cpp` | Semantic GUI/message/Ascii entries; `RUNTIME_ISSUES.md` | ANM resource owner and visual caller are not necessarily the same object. |
| Background and camera | `Background.hpp`, `Background.cpp` | Semantic `Background camera` entry | Camera mode flags are behaviorally observed; adjacent-version labels are secondary evidence. |
| Game state and score | `GameManager.hpp`, `GameManager.cpp` | `GAME_MANAGER_MATCHING.md`, `OWNER_AUDIT.md`, `RUNTIME_ISSUES.md` | Setup/score exact probes duplicate production bodies; canonical storage is the aggregate manager. |
| Title, result, and persistence | `TitleScreen.*`, `ResultScreen.*`, `ScoreDat.*` | `SOURCE_MAP.md`; semantic result/score entries | Title/replay probes are exact-only and shared `.inl` fragments have explicit ownership. |
| Audio and MIDI | `SoundPlayer.*`, `Midi.*`, `Supervisor.*` | Semantic MIDI/streaming-audio entry | Platform backend code under `src/modern/` cannot establish original TH08 semantics. |

For a symbol-level question, search the target address through `src`, `config`,
and `docs`.  The address disambiguates provisional or overloaded names:

```bash
rg -n "0x0045EA00|ExecuteScript" src config docs
```

For build ownership, use `SOURCE_MAP.md`.  For acceptance language and the
required exact/portable oracle pair, use `SEMANTIC_RECONSTRUCTION.md` and
`RE_WORKFLOW.md`.
