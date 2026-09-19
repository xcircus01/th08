# Semantic reconstruction

Semantic reconstruction replaces layout-shaped source—raw object offsets,
anonymous storage, protocol numbers, and provisional identifiers—with
evidence-backed C++ while preserving both the accepted VC7 bytes and playable
modern behavior. Current subsystem navigation lives in
[SEMANTIC_INDEX.md](SEMANTIC_INDEX.md); completed batch records live in
[SEMANTIC_HISTORY.md](SEMANTIC_HISTORY.md). The cross-project lessons distilled
from those batches live in [SEMANTIC_PLAYBOOK.md](SEMANTIC_PLAYBOOK.md).

## Evidence boundary

- **Observed:** TH08 1.00d access offsets and widths, signedness, bit
  operations, callers/callees, strings, relocations, exact object code, and
  verified runtime object identity or behavior.
- **Corroborated:** independent TH08 users agree, or TH06/TH07/upstream naming
  agrees with the TH08 evidence.
- **Inferred:** dataflow supports a neutral role but TH08 does not name it
  independently. State the confidence.
- **Unknown:** only storage, width, or alignment is known. Keep it opaque.

IDA evidence remains blocked until the active database satisfies
`IDA_MCP.md`. Lack of IDA does not lower the evidence bar; use the verified
target, target-safe disassembly, typed fact packets, mappings, and strict
comparison.

## Bounded batch workflow

1. Start with `scripts/analysis/report-semantic-debt.py` and choose one
   structure plus one coherent field/protocol family.
2. Record target functions/addresses and confirm `config/mapping.csv`, the
   relevant `config/reccmp-*.csv`, and `config/match-units.toml` entries.
3. Inspect every read/write, width, caller, and consumer before choosing a
   name. Adjacent versions are supporting evidence only.
4. Capture the affected accepted-unit baseline and determine whether a shared
   header, PCH, inline, layout, or object owner expands the replay scope.
5. Make the smallest natural C++ change, then run the focused VC7 and portable
   oracles.
6. Append the accepted record to `SEMANTIC_HISTORY.md`; update the current
   handoff only if the active phase or blocker changed.

## Source rules

- Preserve VC7 x86 ABI, field offsets and widths, packing, bitfield behavior,
  class size, construction order, translation-unit ownership, and target-shaped
  expressions where code generation depends on them.
- Use `sizeof`/`offsetof` assertions for relied-on layout facts. An assertion
  attests layout, not the English member name.
- Prefer the real aggregate owner. Never invent a second global for an address
  inside an existing object.
- Keep serialization, instruction streams, tagged unions, and platform ABI
  glue byte-oriented where byte addressing is their true representation.
- Do not rename from a numeric offset, one screenshot, one decompiler label, or
  an adjacent game's source alone.
- Do not add aliases, overlays, accessors, or casts merely to make the source
  look typed; they must express an evidenced representation or ownership fact.

## Two-oracle acceptance

| Change | VC7 oracle | Portable oracle |
| --- | --- | --- |
| Private rename/expression in one object | Build the smallest object and replay every accepted unit in it. | Compile/link the configured modern target. |
| Shared header, class layout, inline, PCH, or owner | Replay affected objects, then cold-run `verify-exact-units.py --all`. | Clean compile/link; run fixed-layout/runtime checks when ownership or behavior is involved. |
| Behavior, initialization, persistence, callback, or rendering interpretation | Strictly compare every touched function; a byte regression is not a semantic improvement. | Run the smallest relevant state-transition smoke. |

VC7 exactness proves configured code/data identity, not the chosen English
name. Portable success proves usability on a second implementation, not the
original semantics. Acceptance needs both applicable oracles plus the evidence
record.

## Batch record

After validation, append a concise record to `SEMANTIC_HISTORY.md`:

```text
### Owner / field family — YYYY-MM-DD
Scope: functions @ addresses; source files; VC7 object/profile
Observed: widths, offsets, reads/writes, calls, strings, object identity
Corroborated: independent TH08 users; labeled adjacent-version evidence
Inference: chosen names/types and confidence; unknowns retained
Layout: sizeof/offsetof assertions added or already present
VC7 oracle: focused and required aggregate commands/results
Portable oracle: build/layout/runtime command and result, or not applicable
Result: bounded change; no repository-wide claim
```

Promote a compiler pattern to `VC7_ZUN_PATTERNS.md` or `BUILD_MATCHING.md` only
when it generalizes beyond one semantic batch.
