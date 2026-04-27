# Safety Policy

## Startup
- On power-up, state must be Disabled.
- Initial target must be 0.
- No motor output may be emitted during startup.

## State machine
Allowed states:
- Disabled
- Armed
- Fault

## Disabled
- Physical outputs must be zero, inactive, or not connected.
- Closed-loop control must not generate braking current unless explicitly documented.

## Armed
- Enter only by long press.
- Enter only when target == 0.
- Motor profile must be selected and validated.

## Fault
- Output must be zero.
- Fault must require explicit clear operation.

## Prohibited
- Do not add real motor output in the same change as unrelated UI changes.
- Do not document unimplemented safety behavior as implemented.
