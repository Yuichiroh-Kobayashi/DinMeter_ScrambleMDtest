# AGENTS.md

## Project role
This repository is a PlatformIO / Arduino firmware project for M5Stack DinMeter-based motor controller bring-up.

## Must read first
- `docs/canon/minimum_constraints.md`
- `docs/standards/safety_policy.md`
- `docs/standards/coding_standard.md`
- `docs/architecture/safety_state_machine.md`

## Hard rules
- Power-on state must be Disabled.
- Do not emit motor output in bring-up code unless explicitly requested.
- Do not guess hardware specifications.
- If unclear, write `不明` or `未検証`.
- Update manuals when user-visible behavior changes.

## Build and test
- Build: `pio run`
- Upload: `pio run -t upload`
- Monitor: `pio device monitor`

## End-user manual update rule

When you change code that affects user operation, display text, supported hardware, wiring, safety behavior, profiles, or troubleshooting, update the end-user documentation in the same change.

Required documents:
- `docs/manuals/user_manual.md`
- `docs/manuals/quick_start.md` if the change affects basic operation
- `docs/manuals/troubleshooting.md` if the change affects errors or fault handling

Follow:
- `docs/standards/user_manual_policy.md`
- `docs/templates/end_user_manual_template.md`
- `docs/standards/safety_policy.md`
- `docs/hardware/*/spec.md`

Do not document unimplemented features as available.
If a behavior is unknown or not verified, write `未検証` or `不明`.

## Task skills
For repeated tasks, consult `agent-skills/*/SKILL.md`.
