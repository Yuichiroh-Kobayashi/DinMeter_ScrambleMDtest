# I/O Profile Matrix

## Purpose

この文書は、DinMeter ScrambleMD Test における各モータProfileのI/O割当方針を定義する。

`docs/hardware/*/spec.md` はデバイス仕様を記録する。  
この文書は、プロジェクトとしての仮割当・確定割当を記録する。

## Assignment Status

| Status | Meaning | Code use |
|---|---|---|
| Candidate | 候補。まだ実装既定値にしない | `-1` または未使用 |
| Provisional | 仮割当。コード定数に入れてよいが、実出力は無効のまま | 定数化可 |
| Verified | 実機・ロジアナ・オシロで確認済み | 実出力試験可 |
| Reserved | 他用途で予約 | 使用禁止 |

## Current I/O Matrix

| Profile | Interface | DinMeter Port | GPIO | Status | Notes |
|---|---|---|---:|---|---|
| MD20A_PWM | PWM | PORT.B Yellow | G2 | Provisional | CytronMotorDriver `PWM_DIR` 実出力前にロジアナ確認 |
| MD20A_DIR | DIR | PORT.B White | G1 | Provisional | CytronMotorDriver `PWM_DIR` 実出力前にロジアナ確認 |
| MAKER_DRIVE_M1A | PWM | PORT.B Yellow | G2 | Provisional | CytronMotorDriver `PWM_PWM` |
| MAKER_DRIVE_M1B | PWM | PORT.B White | G1 | Provisional | CytronMotorDriver `PWM_PWM` |
| MAKER_DRIVE_M2A | PWM | PORT.A Yellow | G15 | Provisional | 2ch使用時 |
| MAKER_DRIVE_M2B | PWM | PORT.A White | G13 | Provisional | 2ch使用時 |
| C620_CAN_TX | CAN TX | PORT.A Yellow | G13 | Candidate | 外付けCAN transceiver必須 |
| C620_CAN_RX | CAN RX | PORT.A White | G15 | Candidate | 外付けCAN transceiver必須 |
| EDULITE_CAN_TX | CAN TX | PORT.A Yellow | G13 | Candidate | C620と同じCAN adapter想定 |
| EDULITE_CAN_RX | CAN RX | PORT.A White | G15 | Candidate | C620と同じCAN adapter想定 |
| DDT_RS485_TX | UART TX / RS485 | PORT.B Yellow | G2 | Candidate | MD20Aと同時使用しない |
| DDT_RS485_RX | UART RX / RS485 | PORT.B White | G1 | Candidate | DE/RE制御ピンは未確定 |
| AIR40A_PWM | Servo PWM | PORT.B Yellow | G2 | Candidate | Forward-only。実機校正未実施 |

## Policy

- 同時接続はしない前提なので、Profileごとのピン再利用は許可する。
- ただし、コード上は現在選択中Profile以外の出力を安全値に固定する。
- `Provisional` はコード定数として使ってよい。
- `Verified` になるまで、実モータ接続試験は行わない。
- `Candidate` はコード定数にしてよいが、既定Profileの実出力には使わない。
