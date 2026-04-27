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

## Task skills
For repeated tasks, consult `agent-skills/*/SKILL.md`.
