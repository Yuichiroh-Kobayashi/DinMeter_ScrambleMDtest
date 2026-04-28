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

## Official library rule

Before implementing a motor driver manually, check whether an official library exists.

If an official manufacturer library exists:

1. Add or update `docs/software/<library>/spec.md`.
2. Use the official library through a project wrapper class.
3. Do not call the library directly from UI or safety state code.
4. Preserve the project safety rules:
   - no output in constructor
   - no output in `begin()`
   - no direct output in `setTargetPercent()`
   - output only in `update()` while Armed
   - zero output in Disabled / Fault
5. If the official library is not compatible with ESP32-S3, record the issue and fall back to a custom implementation only after review.

## Pin assignment rule

Before choosing GPIO pins, read:

- `docs/architecture/io_profile_matrix.md`
- relevant `docs/hardware/*/spec.md`

If a pin is marked `Provisional` or `Verified`, implement it as a named constant.  
If a pin is only `Candidate`, do not enable it as the default output pin unless the user explicitly requests it.  
If a pin is `Reserved`, do not use it.

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
