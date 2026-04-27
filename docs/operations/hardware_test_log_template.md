# Hardware Test Log Template

## Test ID

- Test ID:
- Date:
- Tester:
- Reviewer / Witness:
- Location:
- Branch:
- Commit SHA:
- Firmware version:

## Test Purpose

何を確認する試験かを1〜3文で書く。

例:
- MD20A profileで、Disabled中にPWMが0であることを確認する。
- C620 CAN profileで、ID1へ0電流指令を送れることを確認する。

## Target Hardware

| Item | Value |
|---|---|
| DinMeter |  |
| MCU | M5StampS3 / ESP32-S3 |
| Motor / Driver |  |
| Adapter board |  |
| Power supply |  |
| Motor power voltage |  |
| Current limit |  |
| Signal interface | CAN / RS485 / PWM / PWM+DIR |
| Termination |  |
| Load condition | 無負荷 / 車輪あり / 実機搭載 / その他 |

## Documents Checked

- [ ] `docs/standards/safety_policy.md`
- [ ] `docs/architecture/safety_state_machine.md`
- [ ] 対象 `docs/hardware/*/spec.md`
- [ ] `docs/operations/bringup_checklist.md`
- [ ] 必要に応じてユーザーマニュアル

## Pre-test Safety Check

- [ ] 電源投入直後がDisabledである。
- [ ] target = 0で起動する。
- [ ] 対象Profileが正しい。
- [ ] 配線を確認した。
- [ ] 電源電圧を確認した。
- [ ] 電流制限を設定した。
- [ ] 回転部が固定されている。
- [ ] 回転部に手やケーブルが接触しない。
- [ ] 指導者またはメンターが立ち会っている。

## Test Procedure

1. 
2. 
3. 
4. 
5. 

## Expected Result

| Step | Expected |
|---|---|
| 1 |  |
| 2 |  |
| 3 |  |
| 4 |  |
| 5 |  |

## Actual Result

| Step | Actual | Pass / Fail |
|---|---|---|
| 1 |  |  |
| 2 |  |  |
| 3 |  |  |
| 4 |  |  |
| 5 |  |  |

## Measurements

| Item | Value | Unit | Notes |
|---|---:|---|---|
| Supply voltage |  | V |  |
| Current limit |  | A |  |
| Idle current |  | A |  |
| Peak current |  | A |  |
| Target |  | % / rpm / rad/s |  |
| Actual speed |  | rpm / rad/s |  |
| Temperature |  | ℃ |  |

## Logs

Serial log:

```text
paste log here
```

CAN / RS485 / PWM observation:

```text
paste observation here
```

## Issues Found

| Issue | Severity            | Detail | Next action |
| ----- | ------------------- | ------ | ----------- |
|       | Low / Medium / High |        |             |

## Decision

- [ ] Continue
- [ ] Continue with modification
- [ ] Stop and investigate
- [ ] Roll back

Reason:

## Rollback / Recovery
- Rollback commit:
- Hardware recovery:
- Required fix before next test:

## Notes

自由記述
