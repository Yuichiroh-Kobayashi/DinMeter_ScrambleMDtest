# Readable Code Policy

## Core principle
Code is acceptable only when the next maintainer can quickly understand:
- purpose
- safety effect
- hardware side effects
- failure mode

## Naming
- Encode units in variable names.
- Prefer `targetRadPerSec` over `target`.
- Prefer `currentLimitA` over `limit`.
- Avoid `data`, `value`, `tmp` unless scope is very small.

## Comments
- Do not comment obvious code.
- Comment hardware constraints, safety assumptions, protocol traps, and non-obvious decisions.

## Control flow
- Keep normal path straight.
- Use guard clauses for invalid states.
- Do not hide motor output side effects in helper functions.

## Function design
- Separate UI state, safety state, protocol packing, and physical output.
