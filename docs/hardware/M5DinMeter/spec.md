# M5DinMeter v1.1 AI Hardware Notes

このファイルは、AI/Codex が M5Stack DinMeter v1.1 を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、公式ドキュメント、回路図、実機検証を優先してください。

## 位置づけ

- DinMeter v1.1 は、Stamp-S3Aを内蔵した小型表示・操作デバイスである。
- このプロジェクトでは、簡易モータコントローラのHMIとして使用する。
- 最初のbring-upでは、画面表示、ロータリエンコーダ入力、長押しArmed、Serialログのみ確認する。
- CAN/RS485/PWM出力は別段階で実装する。

## 主要仕様

| 項目 | 値 / 内容 |
|---|---|
| Core | M5Stamp-S3A / ESP32-S3FN8 |
| 表示器 | 1.14 inch ST7789P3 |
| 入力 | ロータリエンコーダ + 押しボタン |
| 音 | Buzzer |
| RTC | BM8563 |
| 電源 | USB / DC / BAT |
| DC入力 | 6〜36 V |
| 外部ポート | PORT.A, PORT.B HY2.0-4P |
| 開発 | Arduino / UiFlow2 / ESP-IDF / PlatformIO |

## 電源保持

- バッテリ給電時は、WAKEボタンまたはRTC割込みで起動する。
- 起動後、初期化プログラムで HOLD(GPIO46) を High にしないと再スリープする。
- 電源OFF処理として、USB外部電源なしの場合は HOLD(GPIO46) を Low にできる。

## 重要ピンマップ

### RTC8563 / Encoder / Buzzer / Button

| 機能 | ESP32-S3 GPIO |
|---|---:|
| RTC SCL | G12 |
| RTC SDA | G11 |
| Encoder B | G40 |
| Encoder A | G41 |
| Buzzer | G3 |
| Encoder Button / WAKE | G42 |

### ST7789P3 Display

| 機能 | ESP32-S3 GPIO |
|---|---:|
| CS | G7 |
| SCK | G6 |
| RS/DC | G4 |
| MOSI | G5 |
| RESET | G8 |
| BL | G9 |

### HY2.0-4P Ports

| Port | Black | Red | Yellow | White |
|---|---|---|---|---|
| PORT.A | GND | 5V | G13 | G15 |
| PORT.B | GND | 5V | G2 | G1 |

## Project I/O Assignment Policy

詳細な割当判断は `docs/architecture/io_profile_matrix.md` を正とする。

| 用途 | 推奨候補 | 備考 |
|---|---|---|
| CAN_TX | PORT.A G13 | 外付けCAN transceiver必須 |
| CAN_RX | PORT.A G15 | 外付けCAN transceiver必須 |
| RS485_TX/RX | PORT.B G2/G1 | 半二重制御ピンが必要なら追加検討 |
| MD20A_PWM | PORT.B G2 | CytronMotorDriver経由。実機での出力波形確認は未検証 |
| MD20A_DIR | PORT.B G1 | CytronMotorDriver経由。実機での方向確認は未検証 |

- 実出力は `ENABLE_REAL_MOTOR_OUTPUT=0` の間は無効。
- 実出力前にロジックアナライザまたはオシロスコープで確認する。

## Safety

- 電源投入直後は `Disabled`。
- 起動時 `target = 0`。
- `target == 0` のときだけ長押しで `Armed`。
- bring-up段階では実出力なし。
- `Disabled` ではCAN/RS485/PWM/DIRを送信しない、または安全値固定。

## 実装メモ

- DinMeter固有処理は `board/DinMeterBoard.*` に集約する。
- UIは `ui/ScreenView.*` と `ui/EncoderInput.*` に分ける。
- `HOLD(GPIO46)` は `setup()` 直後に設定するが、起動前に外部プルアップしない。
- 外部ポートは5V電源を持つが、モータ電源として使わない。
- モータ・ESC・ドライバ電源は必ず外部電源とする。

## 未確定事項

- PORT.A/BのどちらをCAN/RS485/PWMに固定するかは、Motor I/F Adapter設計で確定する。
- CANとRS485を同時実装する場合、DinMeterの外部GPIO数が不足する可能性がある。

## Sources

- M5Stack DinMeter v1.1 official documentation: https://docs.m5stack.com/en/core/DinMeter_v1.1
- M5Stack DinMeter Arduino Quick Start: https://docs.m5stack.com/en/arduino/m5dinmeter/program
