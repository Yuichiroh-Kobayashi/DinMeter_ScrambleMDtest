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
| `session_id` | 記録セッションの識別子 | `log_20260502_1200` |
| `sample_index` | 記録開始からの連番 | `0`, `1`, `2` |
| `timestamp_s` | 起動時からの経過時間(秒)。人間や一般のグラフツールが読みやすい形式 | `12.500` |
| `timestamp_ms` | 起動時からの経過時間(ms)。プログラムによる処理や微小時間差の計算に用いる | `12500` |
| `event` | 状態遷移や操作などのイベント名 | `Armed`, `TargetChanged` |
| `safety_state` | 現在の安全状態 | `Disabled`, `Armed`, `Fault` |
| `requested_command` | ユーザーまたは上位システムからの要求値 | `10.0` |
| `applied_command` | 安全制限等を加味して実際にモータへ適用した指令値 | `5.0` |
| `output_state` | モータの出力状態（ON/OFFなど） | `Enabled`, `Stopped` |
| `motor_model` | 接続されているモータの種類 | `EDULITE05`, `DDT_M0602` |
| `driver_mode` | ドライバの制御モード | `Velocity`, `Torque` |
| `measurement_path_code` | データの取得経路 | `Telemetry_CAN`, `Telemetry_RS485` |
| `command_value` | 指令値の数値 | `10.5` |
| `command_semantics` | 指令値の意味と単位 | `Velocity_rad_s`, `Velocity_rpm` |
| `torque_value` | トルクの数値 | `1.2` |
| `torque_semantics` | トルク値の意味と単位 | `estimated_torque_nm`, `measured_torque_nm`, `unknown` |
| `position_value` | 位置の数値 | `3.14` |
| `position_semantics` | 位置値の意味と単位 | `Absolute_rad`, `Turns` |
| `velocity_value` | 速度の数値 | `15.0` |
| `velocity_semantics` | 速度値の意味と単位 | `Feedback_rad_s`, `Feedback_rpm` |
| `voltage_value` | 電圧の数値 | `24.5` |
| `voltage_semantics` | 電圧値の意味と単位 | `BusVoltage_V` |
| `current_value` | 電流の数値 | `0.8` |
| `current_semantics` | 電流値の意味と単位 | `phase_current_a`, `bus_current_a`, `q_axis_current_a`, `unknown` |
| `temperature_value`| 温度の数値 | `45.0` |
| `temperature_semantics` | 温度値の意味と単位 | `MotorTemp_C`, `DriverTemp_C`, `unknown` |
| `power_value` | 電力の数値 | `12.0` |
| `power_semantics` | 電力値の意味と単位 | `EstimatedPower_W` |
| `load_condition` | 負荷の状態（既知の場合） | `NoLoad`, `SpringLoad` |
| `mechanical_state` | 機械的な状態 | `Free`, `Stalled` |
| `abnormal_flag` | 異常検知フラグ | `0`, `OverTemp`, `CommTimeout` |
| `note` | 備考や特記事項 | |

## 値の取り扱いポリシー

- モータからTelemetryとして電流値が得られた場合、それが「相電流（Phase Current）」なのか「バス電流（Bus Current）」なのかを `current_semantics` で明確にする。相電流をそのままトルクとして扱ってはいけない。
- 電流からトルクを推定する場合は、以下のように別々に記録すること：
  - 電流値列と `current_semantics`
  - トルク推定量列と `torque_semantics=estimated_torque_nm`
  - 推定方法を `note`（または将来追加する `torque_estimation_method` 列）に記録する
- **`unknown` の扱い**:
  - semanticsが不明な場合は、空欄にせず明示的に `unknown` と記載する。
  - 値自体が未取得（または取得不可）の場合は、スキーマで「空欄」または `nan` のどちらかに固定して記録する。
  - **`unknown` のsemanticsを持つ値は、教材判断や考察には一切使用しないこと。**
