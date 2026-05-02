---
name: safety-reviewer
description: Use when reviewing changes that affect motor output, state transitions, hardware interfaces, CAN, RS485, PWM, PWM/DIR, fault handling, or user safety documentation.
---

# Safety Reviewer Skill

## Required reading

- `AGENTS.md`
- `docs/canon/minimum_constraints.md`
- `docs/standards/safety_policy.md`
- `docs/standards/review_checklist.md`
- `docs/architecture/safety_state_machine.md`
- `docs/architecture/driver_interface.md`

## Procedure

1. Identify whether the change can affect physical output.
2. Confirm startup is `Disabled`.
3. Confirm `Disabled` and `Fault` force zero output or no command.
4. Confirm `Fault` state does not hold a non-zero target.
5. Confirm `Armed` entry requires long press and target == 0.
6. Confirm `begin()` and constructors emit no physical output.
7. If `ENABLE_REAL_MOTOR_OUTPUT=1` is being proposed:
   - Check for the presence of `docs/operations/motor_output_verification_log.md`.
   - Confirm that at least the `No-motor Waveform` and `No-load Motor` verifications are logged and passed.
8. Confirm documentation updates are included for user-visible changes.
9. If any safety rule is violated, request changes.

## Output

- Safety verdict: Pass / Block / Needs verification
- Violated rules, if any
- Required fixes
- Remaining unverified items

## VAMeter-Edu Motor Observe handoff

Before designing DD motor constant-force control, read:

- `docs/references/vameter_edu_motor_observe_handoff.md`

This handoff defines:

- why VAMeter-Edu + MAKER-DRIVE + gearmotor is withdrawn from the main constant-force device path
- why PWM value and driver input current must not be treated as force or torque
- which safety, CSV, transfer, and Go/NoGO design assets must be reused
- why RobStride EDULITE 05 and DDT-M0602C234 are the next main candidates
