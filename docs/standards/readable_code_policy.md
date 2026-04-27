# AI Agent Coding Policy: ScrambleMD Test

## 0. Project Context (CRITICAL)
- **Target Hardware:** M5Stack DinMeter v1.1 (Stamp-S3)
- **Phase:** Initial Bring-up. Real motor output is NOT fully implemented yet.
- **Goal:** Safe, modular bring-up of various motor drivers (PWM/DIR, CAN, RS485).

## 1. Core Principle
When generating or modifying code, you MUST ensure the output explicitly clarifies:
1. **Purpose:** Which driver/protocol is this for?
2. **Safety Effect:** Does this allow or block motor output?
3. **Hardware Side Effects:** Which GPIOs or buses (CAN/RS485) are touched?
4. **Failure Mode:** What happens on state fault or communication loss?

## 2. Naming Conventions
- **Encode units and physical quantities in variables.**
  - DO: `targetRadPerSec`, `currentLimitA`, `pwmDutyPct`
  - DON'T: `target`, `limit`, `value`
- **Avoid generic names strictly.** Variables like `data`, `tmp`, or `val` are PROHIBITED unless the block scope is under 10 lines.

## 3. Comments
- **DO NOT** explain WHAT the code does (obvious syntax).
- **DO** explain WHY: Document hardware constraints, safety assumptions, protocol traps (e.g., "Robomaster C620 requires ID 0x200"), and non-obvious design choices.

## 4. Control Flow
- Keep the normal path straight (minimize nesting).
- Use early returns (`guard clauses`) to trap invalid states immediately.
- **NEVER** hide physical motor output side-effects inside generic helper functions.

## 5. Architectural Separation
Strictly isolate the following domains. Do not mix them in a single class or function:
1. **UI/Display:** M5GFX/M5Unified screen updates.
2. **Safety Logic:** `Armed` vs `Disabled` state transitions (based on `AppState.h`).
3. **Protocol Logic:** Packing/parsing CAN/RS485 frames.
4. **Physical I/O:** `ledcWrite`, `CAN.write`, etc.

## 6. Embedded Safety (ABSOLUTE RULES)
- **No Output on Init:** Constructors and `begin()` methods MUST NOT emit physical motor commands.
- **Explicit Naming:** Any function that changes physical output MUST be explicitly named (e.g., `writeMotorOutput()`).
- **Disabled = Safe:** The `Disabled` or `Fault` state MUST strictly guarantee that physical output is 0 (or uncommanded/high-Z).
