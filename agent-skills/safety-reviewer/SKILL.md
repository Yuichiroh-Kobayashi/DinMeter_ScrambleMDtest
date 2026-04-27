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
4. Confirm `Armed` entry requires long press and target == 0.
5. Confirm `begin()` and constructors emit no physical output.
6. Confirm documentation updates are included for user-visible changes.
7. If any safety rule is violated, request changes.

## Output

- Safety verdict: Pass / Block / Needs verification
- Violated rules, if any
- Required fixes
- Remaining unverified items
