# MAKER-DRIVE Initial Hardware Test Log

## Test ID

- Test ID: MAKER-DRIVE-INITIAL-001
- Date: 未記入
- Tester: 未記入
- Reviewer / Witness: 未記入
- Location: 未記入
- Branch: dev/bringup
- Commit SHA: 未記入
- Firmware version: 未記入

## Test Purpose

MAKER-DRIVE wrapperで、既定状態では物理出力が無効であることを確認する。実出力を有効化する場合は、Disabled中に停止相当であり、Armed中だけtargetに応じてCytronMotorDriver `PWM_PWM` の `setSpeed()` が呼ばれることを段階的に確認する。

## Target Hardware

| Item | Value |
|---|---|
| DinMeter | M5Stack DinMeter v1.1 |
| MCU | M5StampS3 / ESP32-S3 |
| Motor / Driver | Cytron MAKER-DRIVE / 小型DCモータ |
| Adapter board | 不明 |
| Power supply | 乾電池または電流制限付き電源 |
| Motor power voltage | 2.5〜9.5 V |
| Current limit | 1 A以下から開始 |
| Signal interface | PWM_PWM |
| Load condition | 無負荷から開始 |

## Documents Checked

- [ ] `docs/standards/safety_policy.md`
- [ ] `docs/architecture/safety_state_machine.md`
- [ ] `docs/hardware/MAKER_DRIVE/spec.md`
- [ ] `docs/operations/bringup_checklist.md`
- [ ] `docs/manuals/user_manual.md`

## Pre-test Safety Check

- [ ] 電源投入直後がDisabledである。
- [ ] target = 0で起動する。
- [ ] `ENABLE_REAL_MOTOR_OUTPUT` の値を確認した。
- [ ] M1AがGPIO2、M1BがGPIO1に接続されている。
- [ ] 2ch試験時だけ、M2AがGPIO13、M2BがGPIO15に接続されている。
- [ ] 2ch操作UIは未実装であることを確認した。
- [ ] 小型DCモータのみを接続している。
- [ ] 1A連続を超えるモータを接続していない。
- [ ] 回転部が固定されている。
- [ ] 回転部に手やケーブルが接触しない。
- [ ] 指導者またはメンターが立ち会っている。

## Test Procedure

1. `ENABLE_REAL_MOTOR_OUTPUT=0` のままUSBのみでDinMeterを起動する。
2. Serialログで `output=log-only` を確認する。
3. Single motor modeでM1A/M1Bのpreview値だけが変化することを確認する。
4. 実出力を行う場合だけ、GPIO2/GPIO1の出力波形を確認し、`ENABLE_REAL_MOTOR_OUTPUT=1` でビルドする。
5. モータ電源を接続する前に、Disabled中のM1A/M1Bが停止相当であることを確認する。
6. 乾電池または低電圧・電流制限付きで、targetの小さい値からArmed中の動作を確認する。

## Expected Result

| Step | Expected |
|---|---|
| 1 | StateがDisabled、Targetが0で起動する |
| 2 | MAKER-DRIVE previewログがlog-onlyになる |
| 3 | `setSpeed()` は呼ばれずSerial previewのみになる |
| 4 | GPIO2/GPIO1の出力波形確認後にのみ実出力ビルドを行う |
| 5 | Disabled中の出力が停止相当である |
| 6 | Armed中だけtargetに応じてM1が動作する |

## Actual Result

| Step | Actual | Pass / Fail |
|---|---|---|
| 1 |  |  |
| 2 |  |  |
| 3 |  |  |
| 4 |  |  |
| 5 |  |  |
| 6 |  |  |

## Measurements

| Item | Value | Unit | Notes |
|---|---:|---|---|
| Supply voltage |  | V |  |
| Current limit |  | A |  |
| Idle current |  | A |  |
| Peak current |  | A |  |
| Target |  | % |  |
| PWM duty |  | count / % |  |
| Temperature |  | ℃ |  |

## Withdrawal Conditions

以下のいずれかに該当した場合、試験を中止する。

- 意図しない回転が発生した。
- DisabledまたはFaultで停止相当にならない。
- GPIO2/GPIO1の出力波形または方向が確認できない。
- 1A連続を超える電流が流れた。
- 異常発熱、異臭、煙、火花が出た。
- モータ、治具、配線が固定できていない。
- 原因不明の挙動が再現した。

中止後は、原因が特定されるまで実モータ接続試験を再開しない。

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
