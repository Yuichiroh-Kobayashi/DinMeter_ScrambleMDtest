# DDT-M0602C234 AI Hardware Notes

このファイルは、AI/Codex が DDT-M0602C234 を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、添付済み `DDTモータ.PDF` を正としてください。

## 位置づけ

- DDT-M0602C234 は、モータ、エンコーダ、サーボドライバ一体型のDDモータ。
- このプロジェクトでは、RS485接続の速度制御モータとして扱う。
- 初期実装は **速度環モード** を推奨する。

## 主要仕様

| 項目 | 値 / 内容 |
|---|---|
| 定格電圧 | 25.6 VDC |
| 電圧範囲 | 20〜30 VDC |
| 無負荷回転数 | 470 ±15 rpm |
| 定格回転数 | 320 rpm |
| 定格トルク | 0.55 N·m |
| 定格電流 | 1.20 ±0.12 A |
| ストールトルク | ≥1.70 N·m |
| ストール電流 | ≤6.0 A |
| トルク定数 | 0.44 N·m/A |
| 速度定数 | 19.5 rpm/V |
| エンコーダ | 4096 resolution |
| 重量 | 300 g |
| IP等級 | IP43（螺纹孔密封） |
| 動作温度 | -20〜45 ℃ |

## インターフェース

| 線 | 名称 | 内容 |
|---:|---|---|
| 1 | A / DATA+ | RS485 A |
| 2 | B / DATA- | RS485 B |
| 3 | VCC | 電源+ 最大30 VDC |
| 4 | GND | 電源- |

## RS485通信

| 項目 | 値 |
|---|---|
| Baudrate | 57600 |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | None |
| Data length | 10 bytes |
| Reply | 一問一答 |
| Max rate | 500 Hz |
| CRC | CRC-8/MAXIM over DATA[0]〜DATA[8] |

## 制御モード

| Mode | 値 | 指令範囲 | 単位 / スケーリング |
|---|---:|---|---|
| Current loop | 0x01 | -32767〜32767 | -8〜8 A相当（非電機電流範囲） |
| Speed loop | 0x02 | -475〜475 | rpm |
| Position loop | 0x03 | 0〜32767 | 0〜360° |

初期実装では speed loop 固定。

## 基本操作手順

1. 電機IDを確認する。
2. 必要なら電機IDを設定する。
3. モードを速度環 `0x02` に設定する。
4. 速度指令を送る。
5. stop時は速度指令0またはブレーキを送る。

## Protocol 1: 駆動指令

### Send to motor

| Byte | 内容 |
|---:|---|
| 0 | ID |
| 1 | 0x64 |
| 2 | speed/current/position command high |
| 3 | speed/current/position command low |
| 4 | 0 |
| 5 | 0 |
| 6 | acceleration time |
| 7 | brake |
| 8 | 0 |
| 9 | CRC8 |

- acceleration time: speed loopで有効。1rpmごとの加速時間。単位0.1ms。
- brake: `0xFF` でブレーキ、それ以外は非ブレーキ。speed loopで有効。

### Feedback

| Byte | 内容 |
|---:|---|
| 0 | ID |
| 1 | mode |
| 2 | torque current high |
| 3 | torque current low |
| 4 | speed high |
| 5 | speed low |
| 6 | position high |
| 7 | position low |
| 8 | fault code |
| 9 | CRC8 |

## Protocol 2: 追加フィードバック取得

Send:

| Byte | 内容 |
|---:|---|
| 0 | ID |
| 1 | 0x74 |
| 2〜8 | 0 |
| 9 | CRC8 |

Feedback includes winding temperature and U8 position.

## Fault code

| Bit | 内容 |
|---:|---|
| BIT4 | overtemperature |
| BIT3 | stall |
| BIT2 | phase current overcurrent |
| BIT1 | overcurrent |
| BIT0 | sensor fault |

## Protection

- Bus overcurrent: 3A for 8s triggers protection.
- Motor overtemperature: 80℃ threshold, releases below threshold -5℃.
- Phase current: 4.6A for 5s triggers protection.
- Stall: >5s triggers protection.
- Bus/phase/stall protection total 5 times within short intervals causes disabled until power cycle.

## DinMeter接続案

| DDT | DinMeter / Adapter | 備考 |
|---|---|---|
| DATA+ | RS485 A | 外付けRS485 transceiver必須 |
| DATA- | RS485 B | 外付けRS485 transceiver必須 |
| VCC | 外部20〜30V | DinMeterから供給しない |
| GND | 外部GND / signal GND | 必要に応じて共通GND |

## Codex向け実装メモ

推奨クラス名:

```cpp
class Rs485DdtM0602Driver : public MotorDriver {
public:
  bool begin(uint8_t motorId);
  bool setSpeedMode();
  bool arm();
  bool disarm();
  bool setTargetPercent(int target); // -100..100 -> rpm
  bool setSpeedRpm(int16_t rpm);
  void update();
};
```

実装分割:

- `protocols/Crc8Maxim.*`
- `protocols/DdtM0602Protocol.*`
- `drivers/Rs485DdtM0602Driver.*`

## 未確定事項

- DinMeter側のRS485 transceiver型番。
- 半二重RS485のDE/RE制御ピン割当。
- 実機IDの初期値。
- 左右モータID 04/05をそのまま使うか、単体試験用IDへ変更するか。

## Sources

- User-provided file: `DDTモータ.PDF`
