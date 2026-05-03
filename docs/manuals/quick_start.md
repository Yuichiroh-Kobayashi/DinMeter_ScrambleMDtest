# Quick Start

Date: 2026-04-27
Target hardware: M5Stack DinMeter v1.1

## 1. 起動前

1. DinMeterに実モータ電源を接続していないことを確認します。
2. MD20Aを使う場合でも、初回は `ENABLE_REAL_MOTOR_OUTPUT=0` のままにします。
3. 回転部に手やケーブルが触れない状態にします。
4. 指導者またはメンターが立ち会っていることを確認します。

## 2. 起動

1. DinMeterに電源を入れます。
2. 画面で、Stateが `Disabled` であることを確認します。
3. 画面で、Targetが `0` であることを確認します。
4. Serialログで、MD20Aがlog-onlyであることを確認します。

## 3. 操作確認

1. ロータリエンコーダを回します。
2. Targetが `-100` から `100` の範囲で変化することを確認します。
3. Targetを `0` に戻します。
4. ロータリエンコーダのボタンを長押しします。
5. Stateが `Armed` に変わることを確認します。
6. ロータリエンコーダのボタンを短押しまたは長押しします。
7. Stateが `Disabled` に戻ることを確認します。

## 4. MD20A実出力前

1. `docs/operations/bringup_checklist.md` を確認します。
2. `docs/operations/md20a_initial_test_log.md` を記入します。
3. MD20AのPWMがPORT.B G2、DIRがPORT.B G1に接続されていることを確認します。
4. 出力波形と方向が未検証の場合、実モータ電源を接続しないでください。
5. 実出力を有効化する場合だけ、`ENABLE_REAL_MOTOR_OUTPUT=1` にします。

## 5. MAKER-DRIVE実出力前

1. コンパイル時に `include/config/FeatureFlags.h` で `DEFAULT_MOTOR_PROFILE_MAKER_DRIVE 1` を設定し、プロファイルを切り替えます。
2. `docs/operations/bringup_checklist.md` を確認します。
3. まずは外部電源やモータを接続せず、USB接続のみでのLog-only確認を行います。
4. `docs/operations/maker_drive_initial_test_log.md` またはそれに準ずるログを確認します。2026-05-03に実機での双方向回転が確認済みです。
5. 実出力を有効化する場合、M1AがGPIO2 (Yellow)、M1BがGPIO1 (White) に接続されていることを確認します。
6. 小型DCモータ（1A連続以下）のみを接続します。
7. 実出力を有効化する場合だけ、`ENABLE_REAL_MOTOR_OUTPUT=1` にします。
