---
name: motor-driver-profile
description: Use when adding or modifying a motor driver profile such as MD20A, C620, EDULITE, DDT, or AIR40A.
---

# Motor Driver Profile Skill

## Required reading
- `AGENTS.md`
- `docs/standards/safety_policy.md`
- `docs/architecture/driver_interface.md`
- relevant `docs/hardware/*/spec.md`

## Procedure
1. Identify the target device.
2. Confirm voltage, signal type, protocol, and safe default output.
3. Add profile constants without enabling physical output.
4. Add protocol packing separately from physical transmission.
5. Add user manual notes if operation changes.

## Prohibited
- Do not guess protocol values.
- Do not enable motor output without explicit test plan.
