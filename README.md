# DinMeter ScrambleMD Test

M5Stack DinMeter v1.1 を使って、各種モータ・モータドライバを安全にBring-upするための実験用ファームウェアです。

このリポジトリは、Scrambleジュニアロボットチーム向けに、DDモータやモータドライバを「1台ずつ接続して、正逆転と速度指令を確認する」簡易コントローラを開発するためのものです。

> 現在は初期Bring-up段階です。  
> 実モータ、ESC、CAN、RS485、PWM/DIRへの出力はまだ実装していません。

---

## 目的

現代のDDモータやサーボ系モータは、DCブラシ付きモータのように電源をつなぐだけでは安全に回せません。

このプロジェクトでは、M5Stack DinMeterを操作器として使い、次のようなモータ・ドライバを順次Bring-upできる共通基盤を作ります。

| 対象 | 制御方式 | 初期方針 |
|---|---|---|
| Cytron MD20A | PWM + DIR | DCブラシ付きモータ用 |
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

未実装です。

- CAN送信
- RS485送信
- PWM出力
- PWM + DIR出力
- 実モータ制御
- Fault自動遷移
- モータプロファイル選択UI

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
- Bring-up段階では、実モータ出力を追加しない

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
- `M5DinMeter / M5Unified / M5GFX` ライブラリが取得できているか

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
現段階では、`Armed` になっても実モータ出力は行いません。

---

## ディレクトリ構成

現時点の主要ファイルは次の通りです。

```text
.
├── README.md
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
│   └── AppState.h
├── docs/
│   ├── bringup_log.md
│   ├── fallback_arduino_cli.md
│   ├── safety_state_machine.md
│   └── hardware/
│       └── README.md
└── .vscode/
    └── extensions.json
```

今後、AIコーディングエージェント向けに次の構成へ拡張します。

```text
AGENTS.md
CLAUDE.md
GEMINI.md

docs/
  canon/
  standards/
  architecture/
  hardware/
  manuals/
  templates/
  operations/

agent-skills/
  firmware-bringup/
  motor-driver-profile/
  hardware-spec-author/
  user-manual-writer/
  safety-reviewer/
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

1. AI向けMDファイル群の整備
2. ハードウェア仕様 `docs/hardware/*/spec.md` の追加
3. Safety State Machineの整理
4. Driver Interface設計
5. MD20Aの出力スタブ
6. C620 CAN受信・送信スタブ
7. EDULITE 05 CAN Private protocolスタブ
8. DDT RS485スタブ
9. ユーザー向け取扱説明書の作成

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
