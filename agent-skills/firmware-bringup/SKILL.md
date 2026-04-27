---
name: firmware-bringup
description: Use when creating or modifying DinMeter firmware bring-up code, PlatformIO settings, startup checks, display checks, or serial logging.
---

# Firmware Bring-up Skill

## Required reading
- `AGENTS.md`
- `docs/standards/safety_policy.md`
- `docs/architecture/safety_state_machine.md`
- `docs/operations/bringup_checklist.md`

## Procedure
1. Confirm the current state of `platformio.ini`.
2. Confirm startup remains Disabled.
3. Confirm no motor output is added.
4. Build with `pio run`.
5. Update `docs/operations/hardware_test_log_template.md` if test steps change.

## Output
- Code changes
- Build result
- Test steps
- Unverified items
