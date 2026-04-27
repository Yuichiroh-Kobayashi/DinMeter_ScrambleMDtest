---
name: motor-driver-profile
description: Use when adding or modifying a motor driver profile such as MD20A, C620, EDULITE, DDT, or AIR40A. Use this before adding CAN, RS485, PWM, or PWM/DIR output code.
---

# Motor Driver Profile Skill

## Required reading

- `AGENTS.md`
- `docs/standards/safety_policy.md`
- `docs/standards/embedded_cpp_policy.md`
- `docs/standards/naming_units.md`
- `docs/architecture/driver_interface.md`
- relevant `docs/hardware/*/spec.md`

## Procedure

1. Identify the target device and profile name.
2. Confirm voltage, signal type, protocol, scaling, and safe default output.
3. Add protocol constants without enabling physical output.
4. Separate protocol packing/parsing from physical I/O.
5. Ensure `begin()` emits no physical output.
6. Ensure `setTargetPercent()` does not directly write hardware.
7. Add Japanese Doxygen comments to public driver API.
8. Update user manual notes if operation changes.

## Prohibited

- Do not guess protocol values.
- Do not enable motor output without explicit test plan.
- Do not mix real output implementation with unrelated UI cleanup.
