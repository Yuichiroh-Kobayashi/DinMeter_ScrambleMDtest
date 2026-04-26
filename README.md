# DinMeter ScrambleMD Test

M5Stack DinMeter v1.1（内蔵 Stamp-S3A / ESP32-S3）向けの、簡易モータコントローラbring-up用PlatformIOプロジェクトです。

今回の目的は、Windows上のVS Code + PlatformIOでビルド、書き込み、シリアルログ、DinMeter画面表示を確認することです。実モータ、ESC、CAN、RS485、MD20Aへの出力は実装していません。

## Safety

- 電源投入直後は必ず `Disabled`
- 起動時 `target = 0`
- 長押しでのみ `Armed` に遷移
- `target == 0` のときだけ `Armed` 可能
- `Disabled` ではモータ出力なし
- Bring-up段階ではCAN、RS485、PWM/DIR、ESC信号を一切送信しない

## Environment

- Windows
- VS Code
- PlatformIO IDE extension
- PlatformIO IDE Terminal または PowerShell

WSLは使用しません。

## PlatformIO env

`platformio.ini` の既定envは次の通りです。

```ini
[env:m5stack-stamps3]
platform = espressif32
board = m5stack-stamps3
framework = arduino
```

DinMeter専用のPlatformIO board定義は未確認です。PlatformIO公式ドキュメントで確認できるM5Stack StampS3 board ID `m5stack-stamps3` を使用しています。

## Build

VS CodeのPlatformIO IDE Terminal、またはPowerShellでこのリポジトリを開きます。

```powershell
pio --version
pio project config
pio run
```

## Upload

DinMeterをUSBで接続してCOMポートを確認します。

```powershell
pio device list
pio run -t upload
```

書き込みに失敗する場合は、次を確認します。

- `pio device list` にCOMポートが表示されるか
- USBケーブルがデータ通信対応か
- USB Serial/JTAGドライバが認識されているか
- 必要に応じてStamp-S3AのG0ボタンを押しながら電源投入し、Download Modeに入るか
- バッテリ駆動時にHOLD(GPIO46)が起動直後にHIGHになっているか

## Serial Monitor

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

## Display Check

DinMeter画面に次が表示されます。

- `DinMeter Motor Console`
- `State: Disabled`
- `Target: 0`
- `Output: not connected`

ロータリーエンコーダを回すと `Target` が `-100` から `100` の範囲で増減します。長押しすると、`target == 0` の場合だけ画面上の状態が `Armed` へ変わります。実出力はありません。
