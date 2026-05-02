# DD Motor CSV Logging Plan

## 目的

DDモータ（EDULITE 05 / DDT-M0602C234等）の制御状態およびTelemetryデータをCSV形式で記録するためのスキーマと運用ルールを定義する。
本計画の核心は「単なる数値の羅列ではなく、その値が何を意味するのか（Semantics）を明記すること」である。

## 厳守事項

1. **`unknown`（未定義・不明確）な値を教材判断に使わない。**
2. **PWM値を力（force）とみなさない。**
3. **driver input current を出力トルクとみなさない。**
4. VAMeter等で motor terminal measurement を行ったと記述しない。

## CSVカラム定義（必須候補列）

後段のデータ分析・グラフ化アプリがデータを正しく解釈できるよう、以下の列を含める。

| 列名 | 説明 | 例 |
|---|---|---|
| `timestamp_ms` | 起動時からの経過時間(ms) | `12500` |
| `event` | 状態遷移や操作などのイベント名 | `Armed`, `TargetChanged` |
| `safety_state` | 現在の安全状態 | `Disabled`, `Armed`, `Fault` |
| `requested_command` | ユーザーまたは上位システムからの要求値 | `10.0` |
| `applied_command` | 安全制限等を加味して実際にモータへ適用した指令値 | `5.0` |
| `output_state` | モータの出力状態（ON/OFFなど） | `Enabled`, `Stopped` |
| `motor_model` | 接続されているモータの種類 | `EDULITE05`, `DDT_M0602` |
| `driver_mode` | ドライバの制御モード | `Velocity`, `Torque` |
| `measurement_path_code` | データの取得経路 | `Telemetry_CAN`, `Telemetry_RS485` |
| `command_semantics` | 指令値の意味と単位 | `Velocity_rad_s`, `Velocity_rpm` |
| `torque_semantics` | トルク値の意味と単位 | `Estimated_Nm`, `PhaseCurrent_A` |
| `position_semantics` | 位置値の意味と単位 | `Absolute_rad`, `Turns` |
| `velocity_semantics` | 速度値の意味と単位 | `Feedback_rad_s`, `Feedback_rpm` |
| `voltage_semantics` | 電圧値の意味と単位 | `BusVoltage_V` |
| `current_semantics` | 電流値の意味と単位 | `PhaseCurrent_A`, `BusCurrent_A` |
| `power_semantics` | 電力値の意味と単位 | `EstimatedPower_W` |
| `load_condition` | 負荷の状態（既知の場合） | `NoLoad`, `SpringLoad` |
| `mechanical_state` | 機械的な状態 | `Free`, `Stalled` |
| `abnormal_flag` | 異常検知フラグ | `0`, `OverTemp`, `CommTimeout` |
| `note` | 備考や特記事項 | |

## 値の取り扱いポリシー

- モータからTelemetryとして電流値が得られた場合、それが「相電流（Phase Current）」なのか「バス電流（Bus Current）」なのかを `current_semantics` で明確にする。
- 取得できないTelemetryや、仕様上意味が不明確な値については、推測で埋めず明示的に空欄にするか、当該機能を利用しない。
