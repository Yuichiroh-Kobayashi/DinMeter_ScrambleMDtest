# VAMeter-Edu Motor Observe Handoff

## Purpose

この文書は、VAMeter-Edu / Motor Observe 開発で得られた知見を、DinMeter_ScrambleMDtest における M5DinMeter + DD motor 定力装置開発へ引き継ぐための参照文書である。

本リポジトリでは、教育用可変型定力装置の本線を、M5DinMeter とモータドライバ内蔵ダイレクトドライブモータの組み合わせへ移行する。

対象候補は次を含む。

- RobStride EDULITE 05
- DDT-M0602C234

これらは `docs/hardware/` 配下の仕様書で管理する。

## Research Context

研究全体の目的は、教育用可変型定力装置の開発である。

VAMeter-Edu / Motor Observe では、次の構成で基礎検証を行った。

- VAMeter-Edu
- MAKER-DRIVE
- 減速機付き130サイズDCモータ
- driver input inline 測定
- CSV記録
- QRまたはローカル転送

この検証により、安全状態、測定経路、CSV記録、転送方式、Go/NoGO記録の設計知見は得られた。

一方で、減速機付き130サイズDCモータ構成は、教育用可変型定力装置の本線候補から外す。

## Final Judgment from VAMeter-Edu Motor Observe

### NoGO: VAMeter alone must not measure motor terminal side

VAMeter単体をMAKER-DRIVEのモータ端子間に直接接続しない。

NoGO:

- VAMeterをMAKER-DRIVEのモータ端子間に直接接続する
- VAMeterでモータ端子電圧を測る
- VAMeterでモータ巻線電流を測る
- VAMeterのV/I/Pをモータ出力側物理量として扱う

理由:

- MAKER-DRIVEのdriver outputはHブリッジ出力である。
- モータ方向により端子電圧の極性が反転する。
- VAMeterで双方向シャント電流が扱える可能性と、逆極性端子電圧を入力してよいことは別問題である。

### GO: driver input inline measurement

当面有効だった測定経路は次である。

```text
power + -> VAMeter IN -> VAMeter OUT -> MAKER-DRIVE VB+
power - -> MAKER-DRIVE VB-
```

この経路で観察できるもの:

- driver input voltage
- driver input current
- driver input power

観察できないもの:

- motor winding current
- motor terminal voltage
- output torque
- pulling force

## Lessons from Gearmotor Test

試験対象は、130サイズDCモータ単体ではなく、減速機付き130サイズDCモータである。

観察結果:

- 停止状態からPWMを上げると、40%まではストール状態
- 50%付近で回転開始
- 回転中にPWMを下げると、20%までは回転維持
- 10%付近でストール
- 最大driver input currentは約0.16 A
- 最大driver input powerは約0.48 W

判断:

この構成では、PWM値やdriver input currentから出力トルクや引っ張り力を線形に推定することは困難である。

理由:

- 減速機の静止摩擦
- 歯車抵抗
- 起動トルク
- 回転開始後のヒステリシス
- PWMと回転状態の非線形性
- driver input currentとmotor winding currentの不一致

## Withdrawal Decision

以下は、教育用可変型定力装置の本線候補から外す。

- VAMeter-Edu + MAKER-DRIVE + 減速機付き130サイズDCモータ
- PWM値から力を推定する構成
- driver input currentから出力トルクを推定する構成
- VAMeterでmotor terminal measurementを行う構成

ただし、以下はDinMeter側へ引き継ぐ。

- safety state設計
- requested command / applied command の分離
- CSV記録
- QRまたはローカル転送
- chunked streaming
- measurement semantics
- Go / NoGO / rollback condition
- unknownを教材判断に使わない方針

## Required Direction for DinMeter_ScrambleMDtest

DinMeter_ScrambleMDtestでは、定力装置の本線候補を次へ移す。

- M5DinMeter
- モータドライバ内蔵ダイレクトドライブモータ
- CANまたはRS485による通信制御
- telemetry取得
- CSV記録
- 低トルク・低速域の安全なbring-up
- 最終的な力制御

具体候補:

- RobStride EDULITE 05
- DDT-M0602C234

## Required DD Motor / Driver Features

DDモータ候補は、次を満たすことが望ましい。

- 低速域で安定して動作する
- 出力トルクの線形性が高い
- 減速機由来の摩擦・バックラッシ・効率変動が少ない
- トルク指令または電流指令で動作できる
- 現在値、速度、位置、電流、電圧、温度などをtelemetryとして取得できる
- CSV等で記録できる
- 安全停止条件を設計できる
- 教室で扱える電圧・電流・サイズである

## CSV Logging Requirements

DinMeter側では、値だけではなく、値の意味をCSVへ入れる。

必須候補列:

- `timestamp_ms`
- `event`
- `safety_state`
- `requested_command`
- `applied_command`
- `output_state`
- `motor_model`
- `driver_mode`
- `measurement_path_code`
- `command_semantics`
- `torque_semantics`
- `position_semantics`
- `velocity_semantics`
- `voltage_semantics`
- `current_semantics`
- `power_semantics`
- `load_condition`
- `mechanical_state`
- `abnormal_flag`
- `note`

注意:

- `unknown` の値を教材判断に使わない。
- `driver input current` を `motor winding current` とみなさない。
- `PWM value` を `force` とみなさない。
- `current` と書く場合は、何の電流かを必ず`semantics`列に記録する。

## Scope Boundary

DinMeterファームウェア開発に含めるもの:

- M5DinMeter側の操作UI
- safety state
- motor command
- telemetry取得
- CSV記録
- QRまたはローカル転送
- Go / NoGOログ

DinMeterファームウェア開発に含めないもの:

- CSVを受信・グラフ化・分析するアプリ
- 授業用ワークシート作成

ただし、後段の分析アプリが扱いやすいように、CSVの列名、単位、意味、時刻、状態、eventを明確に出力する責任はDinMeter側にある。

## Bring-up Phases for DD Motor

1. DDモータとM5DinMeterの通信確認
2. telemetry取得確認
3. トルク指令または電流指令の最小送信
4. CSV記録
5. QRまたはPC転送
6. 無負荷での指令値-応答確認
7. 小負荷での線形性確認
8. 安全停止条件の確認

## Go / NoGO Conditions

### GO

- telemetryが取得できる
- 指令値と応答値の関係を説明できる
- 低速・低トルク領域で安定動作する
- CSVで意味付きデータが残る
- 安全停止条件を定義できる
- 教室で扱える電源・サイズ・運用負荷である

### NoGO

- トルク指令と応答が説明できない
- telemetryが取れない
- 低速で不安定
- 発熱が大きい
- 安全停止が設計できない
- データがCSVで残せない
- 教室電源・安全条件に合わない
- unknown の測定値を教材判断に使う必要がある

## Research Paper Reporting Rules

研究報告で使ってよい表現:

- VAMeter-Edu / Motor Observeにより、小型DCモータのPWM制御、ドライバ入力側電流の観察、CSV記録、QR転送の基礎を確認した。
- 一方、減速機付きモータでは静止摩擦や減速機損失により、PWM値やドライバ入力電流から出力力を線形に推定することは困難であった。
- VAMeter単体ではHブリッジ出力の正逆両方向電圧を測定することができないため、モータ巻線電流の直接測定には別の測定デバイスが必要である。
- 以上より、定力装置の本線は、トルク指令とtelemetry取得が可能なダイレクトドライブモータへ移行する。

避ける表現:

- VAMeterでモータ電流を測れた
- VAMeterでモータ端子電圧を測れた
- 減速機付きDCモータで定力制御できる
- PWM値から力が求まる
- driver input currentからトルクが求まる
- Motor Observeは授業用に完成した

## Related Documents

DinMeter側:

- `docs/hardware/RobStride_EDULITE_05/spec.md`
- `docs/hardware/DDT-M0602C234/spec.md`
- `docs/architecture/motor_output_backend_design.md`
- `docs/operations/motor_output_verification_log.md`

VAMeter-Edu側:

- `docs/hardware/VAMeter/measurement_path.md`
- `docs/operations/motor_observe_low_voltage_motor_test_plan.md`
- `docs/operations/safety_test_log.md`
- `docs/hardware/VAMeter_Base/relay_control.md`
- `docs/architecture/motor_observe_measurement_ui_plan.md`
