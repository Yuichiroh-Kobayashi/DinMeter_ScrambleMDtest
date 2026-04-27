# DJI RoboMaster M3508 + C620 AI Hardware Notes

このファイルは、AI/Codex が DJI RoboMaster M3508 P19 + C620 を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、添付済み C620 / M3508 ユーザーガイドを正としてください。

## 位置づけ

- M3508 P19 はブラシレスDCギアモータ。
- C620 は M3508 用のFOC速度コントローラ。
- このプロジェクトでは、DinMeterからC620へCAN指令し、M3508を1台ずつ回す対象として扱う。
- 初期実装は **C620 CAN current command + DinMeter側の簡易速度閉ループ** を推奨する。

## M3508主要仕様

| 項目 | 値 / 内容 |
|---|---|
| 定格電圧 | 24 V |
| 無負荷回転数 | 482 rpm（C620使用時） |
| 無負荷電流 | 0.78 A（C620使用時） |
| 定格回転数 | 469 rpm |
| 定格トルク | 3 N·m（最大連続トルク） |
| 定格電流 | 10 A |
| 最大効率 | 70% |
| ストールトルク | 4.5 N·m |
| 減速比 | 3591/187 |
| 重量 | 365 g |
| 最大巻線温度 | 125 ℃ |

## C620主要仕様

| 項目 | 値 / 内容 |
|---|---|
| 定格電圧 | 24 V |
| 最大連続電流 | 20 A |
| 制御方式 | PWM 50〜500 Hz / CAN bus |
| CAN bitrate | 1 Mbps |
| CAN frame | Standard frame |
| 電源コネクタ | XT30 |
| 注意 | CANケーブルとPWMケーブルを同時接続しない |

## C620 CANプロトコル

### 電流指令

C620のCAN指令は速度指令ではなく、電流指令。

| 対象ID | 送信CAN ID | DLC | 内容 |
|---|---:|---:|---|
| Speed Controller ID 1〜4 | 0x200 | 8 | ID1〜4の電流指令 |
| Speed Controller ID 5〜8 | 0x1FF | 8 | ID5〜8の電流指令 |

- 指令値範囲: `-16384〜0〜+16384`
- 対応トルク電流範囲: `-20〜0〜+20 A`
- 1台だけ使う場合、対象ID以外の2byteは0で埋める。

#### 0x200 data layout

| Byte | 内容 |
|---:|---|
| 0 | ID1 current high byte |
| 1 | ID1 current low byte |
| 2 | ID2 current high byte |
| 3 | ID2 current low byte |
| 4 | ID3 current high byte |
| 5 | ID3 current low byte |
| 6 | ID4 current high byte |
| 7 | ID4 current low byte |

### フィードバック

| 対象 | 受信CAN ID | DLC |
|---|---:|---:|
| Speed Controller ID N | 0x200 + N | 8 |

| Byte | 内容 |
|---:|---|
| 0 | rotor mechanical angle high |
| 1 | rotor mechanical angle low |
| 2 | rotational speed high |
| 3 | rotational speed low |
| 4 | actual torque current high |
| 5 | actual torque current low |
| 6 | motor temperature ℃ |
| 7 | null |

- 角度範囲: 0〜8191 が 0〜360°。
- 速度単位: rpm。
- 温度単位: ℃。
- feedback送信周波数: デフォルト1kHz。

## PWM制御

- C620はPWM 50〜500Hz、1000〜2000us範囲を校正可能。
- 双方向PWMモードでは1500us近傍が0、左右が正逆転。
- 初期実装をCANにする場合でも、PWMケーブルとCANケーブルを同時接続しない。

## 初期制御方針

DinMeterのtargetをrpmへ変換し、C620 feedback rpmと比較して電流指令を出す。

```text
target_percent -> target_rpm
feedback rpm -> actual_rpm
error = target_rpm - actual_rpm
current_cmd_count = clamp(Kp * error + Kd * d_error, -limit, +limit)
```

初期値:

| 項目 | 値 |
|---|---:|
| target_rpm | ±50 rpm から開始 |
| current_limit_count | ±1000 count から開始 |
| Kp | 1.0以下から |
| Kd | 0から |

換算目安:

- 16384 count ≒ 20 A
- 1000 count ≒ 1.22 A
- 2000 count ≒ 2.44 A

## Safety

- 電源投入直後はDinMeter側を `Disabled` とし、C620へ0指令のみ送るか送信しない。
- `Disabled` 中は速度閉ループを回さない。`target_rpm=0` としてPD制御すると制動電流が出る可能性がある。
- `Armed` 遷移時は target=0 を確認する。
- feedback途絶、CAN bus-off、温度異常で `Fault`。
- C620のID重複は出力遮断につながるため、1台試験ではID=1に固定して確認する。

## Codex向け実装メモ

推奨クラス名:

```cpp
class CanC620Driver : public MotorDriver {
public:
  bool begin(uint8_t c620Id);
  bool arm();
  bool disarm();
  bool setTargetPercent(int target);
  void onCanFrame(const CanFrame& frame);
  void update();
};
```

分割:

- `protocols/C620Protocol.*`: frame packing, feedback parsing
- `drivers/CanC620Driver.*`: target mapping, speed loop, safety
- `transport/CanTransportTwai.*`: ESP32-S3 TWAI

## 未確定事項

- DinMeter側で使う制御ゲイン。
- 速度閉ループ周期。
- C620 ID設定手順をUIに入れるか、事前設定前提にするか。

## Sources

- User-provided file: `RoboMaster C620 Brushless DC Motor Speed Controller V1.01.pdf`
- User-provided file: `RoboMaster M3508 P19 Brushless DC Gear Motor V1.0.pdf`
