# Troubleshooting

Date: 2026-04-27
Target hardware: M5Stack DinMeter v1.1

## 1. 起動後にDisabledにならない

1. すぐにDinMeterの電源を切ります。
2. 書き込んだファームウェアのブランチとビルド設定を確認します。
3. 原因が分かるまで実モータ電源を接続しないでください。

## 2. Armedにできない

1. 画面で、Targetが `0` であることを確認します。
2. Stateが `Fault` ではないことを確認します。
3. ロータリエンコーダのボタンを長押ししていることを確認します。
4. Serialログに `MD20A Safety` または `MD20A arm failed` が出ていないか確認します。

## 3. `MD20A arm failed` が出る

1. Targetが `0` であることを確認します。
2. `ENABLE_REAL_MOTOR_OUTPUT` の値を確認します。
3. 実出力有効時は、MD20AのPWMがPORT.B G2、DIRがPORT.B G1に接続されていることを確認します。
4. 出力波形と方向が未検証の場合、実モータ電源を接続しないでください。

## 4. `GPIO is 未検証 or invalid` が出る

1. DinMeterをDisabledのままにします。
2. 実モータ電源を切ります。
3. `include/config/PinConfig.h` のMD20A PWM/DIR GPIO番号を確認します。
4. PWMがPORT.B G2、DIRがPORT.B G1に割り当てられていることを確認します。

## 5. モータが回らない

既定状態ではMD20Aの実出力は無効です。

1. `ENABLE_REAL_MOTOR_OUTPUT=0` の場合、モータが回らない動作が正常です。
2. Serialログで `output=log-only` を確認します。
3. 実出力試験を行う場合は、`docs/operations/md20a_initial_test_log.md` に従ってください。

## 6. 意図しない回転が起きた

1. すぐにStateを `Disabled` にします。
2. モータ用電源を切ります。
3. DinMeterの電源を切ります。
4. 配線、GPIO番号、feature flag、target値を記録します。
5. 原因が特定されるまで実機接続試験を再開しないでください。

## 7. MAKER-DRIVEが動かない

既定状態ではMAKER-DRIVEの実出力は無効です。

1. `ENABLE_REAL_MOTOR_OUTPUT=0` の場合、モータが回らない動作が正常です。
2. 現時点ではMAKER-DRIVEを画面から選択するUIは未実装です。
3. Single motor modeではM1A/M1Bのみを確認します。
4. 2ch modeは構造のみで、UIから使う機能は未実装です。
5. 実出力試験を行う場合は、`docs/operations/maker_drive_initial_test_log.md` に従ってください。
