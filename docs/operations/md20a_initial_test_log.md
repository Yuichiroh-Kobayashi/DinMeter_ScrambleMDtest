# MD20A Initial Hardware Test Log

## Test ID

- Test ID: MD20A-INITIAL-001
- Date: 未記入
- Tester: 未記入
- Reviewer / Witness: 未記入
- Location: 未記入
- Branch: dev/bringup
- Commit SHA: 未記入
- Firmware version: 未記入

## Test Purpose

MD20A PWM+DIRプロファイルで、既定状態では物理出力が無効であることを確認する。実出力を有効化する場合は、Disabled中にPWM=0相当であり、Armed中だけtargetに応じたPWM+DIRが出ることを段階的に確認する。

## Target Hardware

| Item | Value |
|---|---|
| DinMeter | M5Stack DinMeter v1.1 |
| MCU | M5StampS3 / ESP32-S3 |
| Motor / Driver | Cytron MD20A / モータは未記入 |
| Adapter board | 不明 |
| Power supply | 電流制限付き電源を使用 |
| Motor power voltage | 未記入 V |
| Current limit | 未記入 A |
| Signal interface | PWM+DIR |
| Termination | 該当なし |
| Load condition | 無負荷から開始 |

## Documents Checked

- [ ] `docs/standards/safety_policy.md`
- [ ] `docs/architecture/safety_state_machine.md`
- [ ] `docs/hardware/MD20A/spec.md`
- [ ] `docs/operations/bringup_checklist.md`
- [ ] `docs/manuals/user_manual.md`

## Pre-test Safety Check

- [ ] 電源投入直後がDisabledである。
- [ ] target = 0で起動する。
- [ ] `ENABLE_REAL_MOTOR_OUTPUT` の値を確認した。
- [ ] MD20AのPWMがPORT.B G2、DIRがPORT.B G1に接続されている。
- [ ] 出力波形と方向が未検証の場合、実モータ電源を接続していない。
- [ ] MD20Aの逆接保護がないことを確認した。
- [ ] 配線を確認した。
- [ ] 電源電圧を確認した。
- [ ] 電流制限を設定した。
- [ ] 回転部が固定されている。
- [ ] 回転部に手やケーブルが接触しない。
- [ ] 指導者またはメンターが立ち会っている。

## Test Procedure

1. `ENABLE_REAL_MOTOR_OUTPUT=0` のままUSBのみでDinMeterを起動する。
2. Serialログで `output=log-only` を確認する。
3. Disabled中にtargetを変更し、物理出力を接続せずSerialログだけが変化することを確認する。
4. 実出力を行う場合だけ、PORT.B G2/G1の出力波形を確認し、`ENABLE_REAL_MOTOR_OUTPUT=1` でビルドする。
5. モータ電源を接続する前に、オシロスコープまたはロジックアナライザでDisabled中のPWM=0相当を確認する。
6. 低電圧・電流制限付きで、targetの小さい値からArmed中の動作を確認する。

## Expected Result

| Step | Expected |
|---|---|
| 1 | StateがDisabled、Targetが0で起動する |
| 2 | MD20A previewログがlog-onlyになる |
| 3 | GPIO/LEDCへの物理出力は発生しない |
| 4 | PORT.B G2/G1の出力波形確認後にのみ実出力ビルドを行う |
| 5 | Disabled中のPWM dutyが0である |
| 6 | Armed中だけtargetに応じてDIRとPWM dutyが変わる |

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
| PWM frequency |  | Hz |  |
| PWM duty |  | count / % |  |
| Temperature |  | ℃ |  |

## Logs

Serial log:

```text
paste log here
```

PWM / DIR observation:

```text
paste observation here
```

## Withdrawal Conditions

以下のいずれかに該当した場合、試験を中止する。

- 意図しない回転が発生した。
- DisabledまたはFaultでPWMが0相当にならない。
- PORT.B G2/G1の出力波形または方向が確認できない。
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

## Notes

自由記述
