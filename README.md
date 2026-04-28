# DinMeter ScrambleMD Test

M5Stack DinMeter v1.1 を使って、各種モータ・モータドライバを安全にBring-upするための実験用ファームウェアです。

このリポジトリは、Scrambleジュニアロボットチーム向けに、DDモータやモータドライバを「1台ずつ接続して、正逆転と速度指令を確認する」簡易コントローラを開発するためのものです。

> 現在は初期Bring-up段階です。  
> MD20AとMAKER-DRIVE向けにCytron公式ライブラリwrapperを追加しています。  
> ただし既定では `ENABLE_REAL_MOTOR_OUTPUT=0` のため、実モータ出力は行いません。

---

## 目的

現代のDDモータやサーボ系モータは、DCブラシ付きモータのように電源をつなぐだけでは安全に回せません。

このプロジェクトでは、M5Stack DinMeterを操作器として使い、次のようなモータ・ドライバを順次Bring-upできる共通基盤を作ります。

| 対象 | 制御方式 | 初期方針 |
|---|---|---|
| Cytron MD20A | PWM + DIR | CytronMotorDriver `PWM_DIR` wrapper |
| Cytron MAKER-DRIVE | PWM + PWM | CytronMotorDriver `PWM_PWM` wrapper / single motor mode中心 |
| DJI RoboMaster M3508 + C620 | CAN | C620電流指令 + 本機側速度制御 |
| RobStride EDULITE 05 | CAN | Private protocol / Velocity mode |
| DDT-M0602C234 | RS485 | 速度環モード |
| T-MOTOR AIR 40A | PWM | Forward-only扱い |

---

## 現在の実装状態

現在の `dev/bringup` ブランチでは、以下のみを実装しています。

- Windows上の VS Code + PlatformIO でのビルド構成
- M5Stack DinMeter v1.1 / Stamp-S3A 相当のPlatformIO設定
- DinMeterの電源保持 `HOLD(GPIO46)` の初期化
- DinMeter画面表示
- ロータリエンコーダによる `target` 値の変更
- 長押しによる `Disabled` / `Armed` の画面上状態遷移
- 起動ログのSerial出力
- 共通 `MotorDriver` interface
- Cytron MD20A wrapper `CytronMd20aDriver`
- Cytron MAKER-DRIVE wrapper `CytronMakerDriveDriver`
- CytronMotorDriver公式ライブラリ依存
- 実出力無効時のSerial preview

未実装です。

- CAN送信
- RS485送信
- C620 / EDULITE / DDT / AIR40A の実装
- モータプロファイル選択UI
- MAKER-DRIVE 2ch操作UI
- Fault自動遷移
- 実機で検証済みの実モータ制御

---

## 安全仕様

このプロジェクトでは、モータ出力安全を最優先します。

必須仕様:

- 電源投入直後は必ず `Disabled`
- 起動時 `target = 0`
- `target == 0` のときだけ `Armed` へ遷移可能
- 長押しでのみ `Armed` へ遷移可能
- `Disabled` ではモータ出力を出さない
- `Fault` ではモータ出力を出さない
- 既定では `ENABLE_REAL_MOTOR_OUTPUT=0` とし、実モータ出力を行わない
- `ENABLE_REAL_MOTOR_OUTPUT=1` の場合でも、`begin()` は停止値 `setSpeed(0)` だけを明示する
- 非ゼロ出力は `Armed` 状態の `update()` に限定する

禁止事項:

- 未検証のピン・通信・電源仕様を断定しない
- 実モータを接続したまま、未検証コードを書き込まない
- 安全仕様と無関係な変更と、実モータ出力追加を同じコミットに混ぜない
- 非常停止装置の代替として扱わない

---

## 使用環境

想定環境は次の通りです。

- Windows
- VS Code
- PlatformIO IDE extension
- PlatformIO IDE Terminal または PowerShell
- M5Stack DinMeter v1.1

WSLは使用しません。

---

## PlatformIO設定

現在の既定環境は次の通りです。

```ini
[platformio]
default_envs = m5stack-stamps3

[env:m5stack-stamps3]
platform = espressif32
board = m5stack-stamps3
framework = arduino
monitor_speed = 115200
upload_speed = 921600
lib_deps =
  m5stack/M5DinMeter
  m5stack/M5Unified
  m5stack/M5GFX
  https://github.com/CytronTechnologies/CytronMotorDriver.git
build_flags =
  -DARDUINO_USB_CDC_ON_BOOT=1
  -DARDUINO_USB_MODE=1
```

DinMeter専用のPlatformIO board定義は未確定です。  
現時点では、DinMeter v1.1 が Stamp-S3A / ESP32-S3 を内蔵していることから、`m5stack-stamps3` を暫定採用しています。

---

## セットアップ

### 1. リポジトリを取得

```powershell
git clone https://github.com/Yuichiroh-Kobayashi/DinMeter_ScrambleMDtest.git
cd DinMeter_ScrambleMDtest
git checkout dev/bringup
```

### 2. VS Codeで開く

```powershell
code .
```

VS CodeでPlatformIO IDE extensionを有効にしてください。

### 3. PlatformIOを確認

```powershell
pio --version
pio project config
```

---

## ビルド

```powershell
pio run
```

ビルドに失敗した場合は、以下を確認してください。

- PlatformIO IDE extensionが有効か
- `platform = espressif32` が取得できているか
- `m5stack-stamps3` boardが解決できているか
- `M5DinMeter / M5Unified / M5GFX / CytronMotorDriver` ライブラリが取得できているか

---

## 書き込み

DinMeterをUSBで接続し、COMポートを確認します。

```powershell
pio device list
pio run -t upload
```

書き込みに失敗する場合は、以下を確認してください。

- USBケーブルがデータ通信対応か
- WindowsのデバイスマネージャでCOMポートが見えるか
- `pio device list` に対象ポートが表示されるか
- 必要に応じてStamp-S3AをDownload Modeに入れられるか
- USB Serial/JTAGドライバが認識されているか

---

## シリアルモニタ

```powershell
pio device monitor
```

起動時に次のようなログが出ます。

```text
--- DinMeter Motor Console Start ---
Board: M5Stack DinMeter v1.1 (Stamp-S3)
Status: Power Hold (GPIO46) set to HIGH
Setup Complete.
```

終了は `Ctrl+C` です。

---

## 画面表示

DinMeter画面に次の情報を表示します。

- `DinMeter Motor Console`
- `State: Disabled`
- `Target: 0`

ロータリエンコーダを回すと、`Target` が `-100` から `100` の範囲で変化します。

`Target` が `0` の状態でボタンを長押しすると、画面上の状態が `Armed` へ変わります。  
既定設定では、`Armed` になっても実モータ出力は行いません。

---

## 現在のモータドライバwrapper

現時点では、Cytron公式 `CytronMotorDriver` ライブラリをproject wrapper経由で扱います。UIから複数profileを選択する機能は未実装です。

| Wrapper | 対象 | Cytron mode | 現状 |
|---|---|---|---|
| `CytronMd20aDriver` | MD20A | `PWM_DIR` | mainから接続済み。既定はlog-only |
| `CytronMakerDriveDriver` | MAKER-DRIVE | `PWM_PWM` | wrapper追加済み。現行UIからは未選択 |

MAKER-DRIVEは初学者向け・乾電池駆動向けの小型DCモータ用として扱います。2.5〜9.5 V、1 A連続を超える用途には使わないでください。2ch modeの構造はありますが、操作UIと実機検証は未実装です。

---

## ディレクトリ構成

現時点の主要ファイルは次の通りです。

```text
.
├── README.md
├── AGENTS.md
├── CLAUDE.md
├── GEMINI.md
├── platformio.ini
├── src/
│   ├── main.cpp
│   └── drivers/
│       ├── CytronMd20aDriver.cpp
│       └── CytronMakerDriveDriver.cpp
├── include/
│   ├── AppState.h
│   ├── DriverStatus.h
│   ├── MotorDriver.h
│   ├── config/
│   │   ├── FeatureFlags.h
│   │   └── PinConfig.h
│   └── drivers/
│       ├── CytronMd20aDriver.h
│       └── CytronMakerDriveDriver.h
├── docs/
│   ├── architecture/
│   │   ├── driver_interface.md
│   │   ├── io_profile_matrix.md
│   │   └── safety_state_machine.md
│   ├── canon/
│   ├── hardware/
│   │   ├── M5DinMeter/
│   │   ├── M5StampS3/
│   │   ├── MD20A/
│   │   ├── MAKER_DRIVE/
│   │   ├── M3508_C620/
│   │   ├── DDT-M0602C234/
│   │   ├── RobStride_EDULITE_05/
│   │   └── TMOTOR_AIR_40A/
│   ├── manuals/
│   ├── operations/
│   ├── software/
│   │   └── CytronMotorDriver/
│   ├── standards/
│   └── templates/
├── agent-skills/
└── .vscode/
```

---

## AIエージェント運用方針

このリポジトリでは、AIコーディングエージェントを使った開発を前提にします。

想定エージェント:

- Codex
- Antigravity
- Claude Code

共通方針:

- `AGENTS.md` をAI向け共通入口にする
- `CLAUDE.md` / `GEMINI.md` は互換用の薄いラッパーにする
- 実装規約は `docs/standards/` に置く
- ハード仕様は `docs/hardware/` に置く
- 繰り返し作業は `agent-skills/` に置く
- 不明な仕様は `不明` または `未検証` と書く

---

## 今後の実装予定

優先順は次の通りです。

1. 実機なしでの安全状態とlog-only動作確認
2. MD20A / MAKER-DRIVEのロジックアナライザ確認
3. `ENABLE_REAL_MOTOR_OUTPUT=1` での停止値確認
4. 低電圧・電流制限付きの初回実出力試験
5. モータプロファイル選択UIの設計
6. C620 CAN受信・送信スタブ
7. EDULITE 05 CAN Private protocolスタブ
8. DDT RS485スタブ

---

## 注意

このリポジトリのコードは、現時点では研究・教材開発・Bring-up用です。

競技本番用の完成制御装置ではありません。  
安全認証済みの非常停止装置でもありません。  
実モータを接続する前に、必ず配線、電源電圧、対象プロファイル、出力状態を確認してください。

---

## License

未定。

ライセンスを確定するまでは、再利用条件は未定です。
