---
name: firmware-bringup
description: Use when creating or modifying DinMeter firmware bring-up code, PlatformIO settings, startup checks, display checks, encoder input, or serial logging. Do not use for real motor output implementation.
---

# Firmware Bring-up Skill

## Required reading

- `AGENTS.md`
- `docs/standards/safety_policy.md`
- `docs/standards/embedded_cpp_policy.md`
- `docs/architecture/safety_state_machine.md`
- `docs/operations/bringup_checklist.md`

## Procedure

1. Confirm the task is bring-up only.
2. Confirm real motor output is not added.
3. Check `platformio.ini`.
4. Check startup state remains `Disabled`.
5. Check initial target remains `0`.
6. Build with `pio run`.
7. If user-visible behavior changes, update manuals.

## Output

- Summary of code changes
- Build result
- Test steps
- Unverified items
