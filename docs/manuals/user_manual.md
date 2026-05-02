# DinMeter Scramble Motor Console 取扱説明書

Version: 0.1.0
Date: 2026-04-27
Target hardware: M5Stack DinMeter v1.1

## 1. この装置でできること

この装置は、M5Stack DinMeter v1.1を使って、モータドライバのBring-upを行うための簡易コントローラです。

現時点のMD20A PWM+DIRプロファイルは検証中です。既定のファームウェアでは `ENABLE_REAL_MOTOR_OUTPUT=0` のため、MD20Aへ物理出力せず、Serialログだけを出します。

## 2. 必ず守ること

- 電源投入直後は、画面のStateが `Disabled` であることを確認してください。
- `Target: 0` のときだけArmedにできます。
- Armedにする前に、配線、電源電圧、対象プロファイル、周囲の安全を必ず確認してください。
- モータ、車輪、ギア、回転軸に手やケーブルが近い状態でArmedにしないでください。
- 実験時は、指導者またはメンターが必ず立ち会ってください。
- この装置を非常停止装置の代わりに使わないでください。

## 3. 起動方法

1. DinMeterに電源を入れます。
2. 画面に `DinMeter Motor Console` と表示されることを確認します。
3. 画面で、Stateが `Disabled` であることを確認します。
4. 画面で、Targetが `0` であることを確認します。
5. Serialログで、MD20Aがlog-onlyであることを確認します。

## 4. 基本操作

### targetを変更する

1. DinMeterのロータリエンコーダを回します。
2. 画面の `Target` が変わることを確認します。
3. targetの範囲は `-100` から `100` です。

### Armedにする

1. 画面で、`Target: 0` と表示されていることを確認します。
2. 配線、電源、プロファイル、周囲の安全を確認します。
3. ロータリエンコーダのボタンを長押しします。
4. 画面のStateが `Armed` に変わることを確認します。

### Disabledに戻す

1. ロータリエンコーダのボタンを長押しします。
2. 画面のStateが `Disabled` に変わることを確認します。

Armed中は、ロータリエンコーダの短押しでも `Disabled` に戻ります。

## 5. MD20A PWM+DIRプロファイル

MD20Aは検証中です。既定状態では実モータ出力は有効ではありません。

| 項目 | 内容 |
|---|---|
| 対象 | Cytron MD20A |
| 制御方式 | PWM + DIR |
| target変換 | -100から100をCytronMotorDriverの速度指令 -255から255へ変換 |
| 既定動作 | Serialログのみ。物理出力なし |
| 実出力有効化 | `ENABLE_REAL_MOTOR_OUTPUT=1` が必要 |
| GPIO | PWM: PORT.B G2、DIR: PORT.B G1。実機での出力波形と方向は未検証 |

実出力を有効化する前に、`docs/operations/bringup_checklist.md` と `docs/operations/md20a_initial_test_log.md` に従って確認してください。

## 6. MAKER-DRIVE PWM_PWMプロファイル

MAKER-DRIVEは検証中です。現時点では `DEFAULT_MOTOR_PROFILE_MAKER_DRIVE 1` をコンパイル時に指定することで、Single motor modeのlog-only確認が行えます。画面からプロファイル選択して使うUIは未実装です。DinMeter上での実運転は未実施であり、VAMeter-Eduでの試験結果をDinMeter上の確認済み結果として扱わないでください。

| 項目 | 内容 |
|---|---|
| 対象 | Cytron MAKER-DRIVE |
| 用途 | 初学者向け、乾電池駆動向けの小型DCモータ |
| モータ電圧 | 2.5〜9.5 V |
| 連続電流 | 1 A / ch |
| 制御方式 | CytronMotorDriver `PWM_PWM` |
| 初期対応 | Single motor modeのみ |
| 2ch対応 | 構造のみ。UI未実装、実機未検証 |
| 既定動作 | Serial previewのみ。物理出力なし |
| 実出力有効化 | `ENABLE_REAL_MOTOR_OUTPUT=1` が必要 |
| GPIO | M1A: G2、M1B: G1、M2A: G13、M2B: G15。実機波形と方向は未検証 |

MAKER-DRIVEを高電流用途に使わないでください。1A連続を超えるモータは接続しないでください。

## 7. 停止方法

1. targetを `0` に戻します。
2. Stateを `Disabled` に戻します。
3. モータが停止していることを確認します。
4. モータ用電源を切ります。
5. DinMeterの電源を切ります。

## 8. Fault時の扱い

Fault時は出力しません。原因が不明な場合は、実機接続試験を再開しないでください。

| 表示またはログ | 意味 | 対処 |
|---|---|---|
| `driver arm failed` | ドライバをArmedにできない | target、GPIO設定、feature flagを確認します |
| `GPIO is 未検証 or invalid` | 実出力有効時にGPIOが未設定 | 回路図と実機でGPIOを確認します |

## 9. 未実装または未検証

- MD20Aの実機回転試験は未検証です。
- MD20AのPWM=GPIO2、DIR=GPIO1は仮割当です。実機波形と方向は未検証です。
- MAKER-DRIVEの実機回転試験は未検証です。
- MAKER-DRIVEの2ch UI操作は未実装です。
- C620とDDT-M0602C234は、このPhase 1では実装していません。
