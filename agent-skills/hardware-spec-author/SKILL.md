---
name: hardware-spec-author
description: Use when creating or updating AI-readable hardware specification files under docs/hardware for M5DinMeter, M5StampS3, MD20A, EDULITE, M3508_C620, DDT-M0602C234, or TMOTOR_AIR_40A.
---

# Hardware Spec Author Skill

## Required reading

- `AGENTS.md`
- `docs/canon/minimum_constraints.md`
- `docs/hardware/README.md`
- `docs/templates/hardware_spec_template.md` if present

## Procedure

1. Identify the target device.
2. Separate confirmed facts from unverified assumptions.
3. Record voltage, current, signal level, communication speed, pin assignment, protocol IDs, scaling, and safety notes.
4. Mark unknown items as `不明` or `未検証`.
5. Do not invent missing datasheet values.
6. Add Codex-facing implementation notes only when they follow from confirmed facts.

## Output

- Updated `docs/hardware/<device>/spec.md`
- List of unverified items
- Source list
