# M5StampS3 / Stamp-S3A AI Hardware Notes

このファイルは、AI/Codex が M5Stack Stamp-S3A を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、公式ドキュメントと回路図を優先してください。

## 位置づけ

- M5Stack Stamp-S3A は ESP32-S3FN8 ベースの小型組み込みモジュール。
- DinMeter v1.1 のメインコントローラとして搭載されている。
- このプロジェクトでは、DinMeter の表示・ロータリエンコーダ・外部ポート・通信インターフェース制御の基盤として扱う。

## 主要仕様

| 項目 | 値 / 内容 |
|---|---|
| SoC | Espressif ESP32-S3FN8 |
| CPU | Xtensa LX7 dual-core, 最大 240 MHz |
| Flash | 8 MB |
| 入力電圧 | DC 5 V |
| GPIO | G0/G1/G2/G3/G4/G5/G6/G7/G8/G9/G10/G11/G12/G13/G14/G15/G39/G40/G41/G42/G43/G44/G46 |
| 無線 | 2.4 GHz Wi-Fi |
| 開発環境 | Arduino IDE / UiFlow2 / ESP-IDF / PlatformIO |
| ユーザーI/F | 物理ボタン、WS2812B RGB LED |
| 動作温度 | 0〜40 ℃ |

## 起動・書き込み

- Download Mode: 電源投入前に G0 ボタンを押し続け、通電後に離す。
- G0 と G46 は ESP32-S3 の strapping pin。
- G46 は通常起動時に内部プルダウンされる。**チップ起動前に G46 をHighへ固定しないこと。**
- DinMeterでは、起動後に HOLD(GPIO46) を High にして電源保持する必要がある。

## このプロジェクトでの扱い

- PlatformIO の初期候補 board は `m5stack-stamps3`。
- DinMeter専用board定義が使用可能なら、将来そちらへ切替を検討する。
- `ARDUINO_USB_CDC_ON_BOOT=1` と `ARDUINO_USB_MODE=1` は USB Serial/JTAG 経由のログ確認に有効。

## 実装上の注意

- Stamp-S3A単体仕様と DinMeter実装上のピン割当は分けて扱うこと。
- G46は起動モードにも関係するため、電源保持用途では `setup()` 開始後に設定する。
- ESP32-S3はCANトランシーバを内蔵しない。TWAI(CAN controller)を使う場合も外付けCAN transceiverが必要。
- UART/RS485、PWM、I2C、GPIOはDinMeterのPORT.A/PORT.B経由のピン制約を受ける。

## Codex向け実装メモ

- Stamp-S3Aを直接制御する低レイヤ処理は `board/` に隔離する。
- DinMeter固有のHOLD、Display、Encoder処理は `board/DinMeterBoard.*` または `ui/` に分ける。
- モータ出力系コードから G46 を触らない。

## 未確定事項

- PlatformIOに DinMeter v1.1 専用board定義が存在するかは未確定。
- Stamp-S3Aの全ピンをDinMeter上で自由に使えるわけではない。DinMeter側ピンマップを優先する。

## Sources

- M5Stack Stamp-S3A official documentation: https://docs.m5stack.com/en/core/Stamp-S3A
- M5Stack DinMeter v1.1 official documentation: https://docs.m5stack.com/en/core/DinMeter_v1.1
