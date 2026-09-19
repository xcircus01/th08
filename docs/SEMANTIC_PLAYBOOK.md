# Semantic and readability playbook

This playbook distills the recurring method from TH08's completed semantic
batches into guidance that can be reused by later reconstruction projects. It
does not replace the target-specific workflow, current semantic policy, exact
ledgers, or chronological history:

- use [RE_WORKFLOW.md](RE_WORKFLOW.md) for the repository's reconstruction and
  acceptance process;
- use [SEMANTIC_RECONSTRUCTION.md](SEMANTIC_RECONSTRUCTION.md) for the current
  TH08 batch rules and validation matrix;
- use [SEMANTIC_INDEX.md](SEMANTIC_INDEX.md) for current TH08 owners and
  declarations;
- use [SEMANTIC_HISTORY.md](SEMANTIC_HISTORY.md) when a decision needs its
  address-backed evidence and Oracle results.

The central rule for a later title is: **transfer the method, never the
conclusion**. A neighboring game can suggest a name, layout, protocol, or
compiler idiom, but the new target must prove it again.

## Start from a semantic question, not a cleanup count

A good batch asks one bounded question, such as:

- who owns this storage and which functions change its lifetime;
- what state transition does this field participate in;
- which values form one serialized protocol;
- whether several raw views are the same record or different namespaces; or
- whether an exact-only probe and the playable build execute the same logic.

Raw-offset, anonymous-name, and numeric-literal scanners are useful routers.
They are not completion metrics. TH08 reached zero in its first structural
categories while ordinary empty production functions, secondary instruction
streams, resource IDs, flag namespaces, and exact-probe duplication still hid
important semantic debt.

Keep the batch to one owner plus one coherent field or protocol family. If the
question expands into unrelated managers, stop and split it. Small batches
make the evidence, compiler response, and validation scope attributable.

## Build an evidence packet and close the consumers

Before choosing a name or type, record a compact evidence packet:

| Evidence | Questions to answer |
| --- | --- |
| Identity | Target version, address or range, current symbol, mapping rows, comparison unit, and source/build owner. |
| Representation | Offset, width, signedness, alignment, packing, bit operations, serialized form, and relocation shape. |
| Producers | Constructors, loaders, setters, decoders, reset paths, and every observed write. |
| Consumers | Readers, branches, arithmetic, calls, renderers, serializers, and every observed use. |
| Transitions | Initial value, discriminators, valid value domain, lifetime, reset point, and ownership transfer. |
| Corroboration | Independent users in the same target, strings/resources, runtime observations, and clearly labeled adjacent-title evidence. |
| Boundary | What is still unknown, which users were not inspected, and what the proposed name deliberately does not claim. |

One write usually proves storage, not meaning. A reset proves that a field is
reset state; it does not prove a gameplay role. One screenshot may distinguish
a visual path, but it rarely proves a globally stable asset name. Close the
producer and consumer set far enough that the proposed concept explains all
observed accesses without contradicting any of them.

Tiny helpers are often better evidence than a large interpreter or boss
function. Recover a small setter, getter, decoder, or callback first, then use
its proven offsets and transitions to type the larger consumer. This reduces
the temptation to name a field from one complicated call site.

## Keep four kinds of ownership separate

The TH08 history repeatedly showed that “owner” is not one question:

| Ownership axis | Meaning |
| --- | --- |
| Storage owner | The aggregate or allocation whose lifetime contains the bytes. |
| Semantic owner | The object or subsystem whose behavior the field or helper describes. |
| Emission owner | The original translation unit or header/COMDAT context that emitted the target code or data. |
| Build owner | The production, exact-probe, or portable source selected for a particular build. |

These axes can agree, but none implies the others. A helper located beside ECL
code may semantically operate on `Enemy`; a class method may have been emitted
from a consumer translation unit; an exact probe may contain accepted behavior
while the production file still has a stub; and a portable aggregate may need
to represent storage that VC7 addressed as a direct symbol.

Practical rules follow:

- prefer the canonical aggregate or allocation; never create a second global
  for an address inside an existing owner;
- do not infer semantic class ownership from target neighborhood alone;
- do not infer translation-unit ownership from the qualified C++ name;
- inspect build selectors before assuming an exact probe is playable code;
- when behavior moves between owners, replay the donor, recipient, and callers,
  then link the real production image.

This separation is especially important before porting. Modern startup glue,
fixed-address shims, or a friendlier linker can hide duplicate storage,
missing production definitions, static-initialization order, and lifetime
errors. The native architecture/compiler build-and-play gate is therefore a
prerequisite Oracle, not a later packaging detail.

## Name only to the strongest evidence boundary

Use a naming ladder and stop at the highest supported rung:

1. physical fact: width, offset, serialized slot, or alignment;
2. lifecycle fact: constructor-cleared, reset-only, borrowed, retained, or
   unconsumed;
3. structural role: counter, index, callback, position, range, or tagged
   parameter;
4. behavioral role: fade state, death mode, score value, draw path, or script
   selector;
5. presentation identity: a specific sound, effect, portrait, or animation.

A neutral name such as `serializedReserved02`, `resetOnlyState829C`, or
`unconsumedSeekResult` is a successful result when that is all the target
proves. Keeping a numeric value can likewise be more accurate than inventing an
enum member for an array index, quantity, stage-provided script number, or
visually ambiguous per-file animation ID.

Names should describe invariant behavior, not the first place a value was
noticed. If the same storage has discriminator-selected roles, use an asserted
tagged representation or a narrow local overlay only when all roles and the
discriminator are evidenced. A union is not a license to hide uncertainty.

## Treat namespaces and protocols as first-class reconstruction work

Structural scans do not see most protocol debt. After fields and owners are
stable, explicitly inventory:

- primary opcodes and operand selectors;
- sibling timeline, stage, background, replay, and mode streams;
- instruction selector bits versus physical object flags;
- callback reasons, scheduler priorities, phases, and tagged parameters;
- stable sound/effect/resource IDs and their complete dispatch domains; and
- file slots, resource-local script IDs, sprite IDs, and runtime VM indices.

Equal numbers do not make two domains the same namespace. TH08 found, for
example, that Background and ANM interpolation modes use related concepts with
different numbering, and that ANM manager slots, script IDs, sprite IDs, and
VM indices need separate vocabularies.

Name a complete observed domain when practical. Include numeric comparisons,
range arithmetic, out-of-order handlers, and writers as well as `case` labels.
Preserve physical handler order when it affects compiler output. Once the
surface is closed, add a focused guard that verifies the value set and decoder
coverage and rejects the actual regression spellings. Test the guard's
accepted and rejected cases; TH08's first stable-ID check missed direct
literals and C-style casts because it covered only one `static_cast` form.

## Preserve representation and compiler-shaped source

Readable source still has to describe the representation that the target used.

- Pin relied-on sizes, offsets, widths, and packing with compile-time
  assertions. These prove layout, not the English name.
- Keep wire formats, bytecode, tagged records, and platform ABI glue
  byte-oriented where byte addressing is the true model.
- Prefer natural fields, aggregates, enums, and bitfields, but do not replace a
  target-observed shift, whole-dword mask, or expression tree merely because a
  prettier formulation is semantically equivalent.
- Avoid an accessor when the original unoptimized build needs a direct load or
  when `/Ob0` would materialize a call.
- For typed wire packets, derive compile-time field indices from `offsetof`
  while retaining the accepted resolver and bit-cast expression shape.
- Introduce one packet family or protocol family at a time and compare the
  complete containing function before extending the pattern.

Source spelling can be evidence. In TH08, an equivalent-looking float read
changed a large interpreter by one byte; lexical switch order changed handler
layout; and direct versus cached aggregate expressions changed relocations.
Do not dismiss a small exact difference as cosmetic until it is classified.

Sometimes one meaning needs two storage expressions. A target-only translation
unit may need the direct symbol observed in VC7 while the portable build routes
the same logical state through an aggregate. Keep that divergence at a narrow,
named build boundary—often a translation-unit-local macro—and keep the function
body semantically identical. Similarly, target-adjacent globals may permit one
contiguous operation without promising adjacency in a modern linker.

## Use independent Oracles and escalate validation deliberately

No single check proves semantics:

| Change surface | Minimum evidence after the edit |
| --- | --- |
| Private expression or name | Rebuild the smallest object and replay every accepted unit in that object. |
| Shared declaration, inline body, PCH, layout, or owner | Replay affected objects, then perform a cold aggregate exact build. |
| Emission/build owner | Replay donor, recipient, and callers; link the native production image. |
| Portable storage or ABI bridge | Build/link the portable target and run fixed-layout checks. |
| Initialization, persistence, callback, rendering, or lifetime | Add the smallest state-transition test or native runtime/playtest that exercises it. |
| Completed protocol surface | Run its guard and focused guard regression tests in CI. |

The Oracles answer different questions:

- exact comparison proves configured target bytes and relocations, not the
  chosen English meaning;
- layout assertions prove representation, not behavior;
- portable compilation proves another compiler can consume the source, not
  that the original owner or ABI was recovered;
- runtime behavior proves the exercised path, not binary identity; and
- adjacent titles corroborate a hypothesis, never prove the current target.

Run the original architecture/compiler whole-program gate before treating a
modern port as validated. TH08's native Windows i386 pass found data-owner,
link-callee, callback-table, initialization, and lifetime defects that had
survived function-level exact checks and modern-port testing.

## Classify a regression before repairing it

When an accepted exact unit changes, first separate:

1. function extent or boundary differences;
2. non-relocation instruction/data byte differences; and
3. relocation offset, type, target, or addend differences.

For a relocation-only change, mask relocation fields on both sides and require
zero remaining byte differences. Infer the target base independently from the
target value and object addend, then reconcile it with the mapping/global
evidence. Preserve intentional addends. Do not weaken a comparator, broaden a
range, or migrate a manifest merely because a new aggregate spelling looks
cleaner.

If a cold aggregate contradicts the accepted ledger but the edited object is
still exact, record a baseline blocker rather than absorbing unrelated repair
work into the semantic batch. A focused pass, a reused build, and an old prose
total cannot support a fresh repository-wide exact claim.

## Define completion as several closures

A semantic pass is not complete merely because a scanner reaches zero. Review
these closures independently:

1. **representation closure:** raw views have justified typed owners or remain
   intentionally byte-oriented;
2. **consumer closure:** names explain the observed producers, consumers, and
   transitions, with unknowns stated;
3. **protocol closure:** opcodes, selectors, flags, resources, and sibling
   streams were inventoried beyond structural offsets;
4. **ownership closure:** storage, semantic, emission, and build owners agree
   with the real production link rather than only an exact probe;
5. **Oracle closure:** focused exact, required cold aggregate, production
   link, portable, and runtime checks passed for their applicable risks;
6. **memory closure:** the result has a canonical record, and a complete stable
   surface has a regression guard when one is practical.

Completion is scoped. It is valid to close one protocol while leaving
ambiguous assets neutral, or to finish semantic readability while one unrelated
authored function remains a known near match.

## Turn discoveries into durable repository memory

Store each kind of knowledge once:

- ledgers and generated reports hold live inventory and exact status;
- a short current policy holds the rules for the next batch;
- a current subsystem/source map answers where code and evidence live now;
- append-only history holds accepted evidence and superseded decisions;
- focused notes hold long compiler or function investigations;
- skills hold repeatable task procedure and stop rules;
- tests and guards enforce completed mechanical surfaces; and
- the handoff contains only the current phase, blocker, and next bounded work.

Do not make chronological history the entry point for ordinary maintenance,
and do not copy live totals into several prose files. When a batch teaches a
general rule, promote the rule to the policy, playbook, skill, or compiler
pattern document; leave the address-backed example in history.

## Cross-title transfer contract

| Safe to carry forward | Must be proved again for every target |
| --- | --- |
| Evidence classes and the consumer-closure checklist | Field names, offsets, widths, signedness, packing, and class layout |
| The four ownership axes | Aggregate identity, translation-unit boundaries, COMDAT emission, and static initialization |
| Validation-scope escalation | Calling conventions, callback signatures, vtables, and exception behavior |
| Scanner, guard, and regression-test patterns | Opcode domains, flag bits, enum values, and resource meanings |
| Policy/index/history/handoff document roles | Function boundaries, relocations, source expression shapes, and exact status |
| The requirement for a native whole-program prerequisite | Runtime initialization, ownership, persistence, rendering, and lifetime behavior |

Pin the adjacent repository revision used for comparison, label every borrowed
name as a hypothesis, and record disagreements. A later game often shares an
engine concept while changing the numeric domain, object boundary, owner, or
compiler-emission context.

## Batch checklist

Before editing:

- state one semantic question and the intended stop boundary;
- verify the target and locate every mapping/comparison/build owner;
- enumerate producers, consumers, transitions, widths, and unknowns;
- separate the four ownership axes and identify the affected Oracles; and
- capture focused accepted-unit and portable baselines.

Before accepting:

- confirm the name does not claim more than the evidence;
- confirm layout assertions cover every relied-on representation fact;
- compare the smallest affected object and all accepted units it contains;
- run every escalated native, aggregate, portable, runtime, and guard check;
- record retained unknowns and rejected interpretations; and
- promote only the reusable lesson, leaving detailed evidence in the batch
  history.
