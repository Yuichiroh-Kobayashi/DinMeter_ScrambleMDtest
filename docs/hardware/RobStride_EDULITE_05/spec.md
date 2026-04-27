# RobStride EDULITE 05 AI Hardware Notes

このファイルは、AI/Codex が RobStride EDULITE 05 を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、添付済み `EL05User Manual251210.pdf` を正としてください。

## 位置づけ

- EDULITE 05 は、モータ、減速機、ドライバ、エンコーダを統合したQDDモータモジュール。
- このプロジェクトでは、CAN接続のDDモータとして扱う。
- 初期実装は **Private protocol / Velocity mode** を推奨する。

## 主要仕様

| 項目 | 値 / 内容 |
|---|---|
| 定格電圧 | 48 VDC |
| 動作電圧範囲 | 15〜60 VDC |
| 定格負荷 | 1.8 N·m |
| 定格負荷速度 | 100 rpm ±10% |
| 無負荷速度 | 430 rpm ±10% |
| ピーク負荷 | 6 N·m |
| 減速比 | 9:1 |
| 駆動方式 | FOC |
| CAN bitrate | 1 Mbps |
| エンコーダ | 14 bit absolute turn |
| 重量 | 242 g ±3 g |
| 動作温度 | -20〜50 ℃ |

## コネクタ / インターフェース

| 端子 | 内容 |
|---|---|
| VBAT+ | モータ電源+ |
| GND | モータ電源- |
| CAN-L | CAN Low |
| CAN-H | CAN High |

- CAN transceiver はDinMeter側に外付けする。
- Motor powerはDinMeterから供給しない。

## プロトコル選択

EDULITE 05 は複数プロトコルを持つ。

| Protocol | CAN frame | 初期採用 | 備考 |
|---|---|---:|---|
| Private protocol | Extended 29-bit | Yes | マニュアルの基本手順が明確 |
| MIT protocol | Standard 11-bit | Later | 切替後、再起動が必要 |
| CANopen | CANopen | Later | CiA402系。初期実装には重い |

## Private protocol 基本

| 項目 | 内容 |
|---|---|
| CAN | CAN 2.0 |
| Bitrate | 1 Mbps |
| Frame | Extended frame |
| Data | 8 bytes |
| 29-bit ID構造 | Bit28..24: communication type / Bit23..8: data area 2 / Bit7..0: destination address |

### 拡張CAN ID生成

```cpp
uint32_t makeEduLiteExtId(uint8_t type, uint16_t data, uint8_t motor_id) {
  return ((uint32_t)type << 24) | ((uint32_t)data << 8) | motor_id;
}
```

## 初期推奨: Velocity mode

### 手順

1. Type 18: `run_mode = 2` を書き込む。
2. Type 18: `limit_cur` を低めに設定する。
3. Type 18: `acc_rad` を低めに設定する。
4. Type 3: Motor Enable。
5. Type 18: `spd_ref` に目標速度 rad/s を周期送信。
6. Type 4: Stop。

### 主要パラメータ

| Index | Name | Type | 内容 | 範囲 / 単位 |
|---:|---|---|---|---|
| 0x7005 | run_mode | uint8 | 制御モード | 2 = Velocity mode |
| 0x700A | spd_ref | float | 速度指令 | -50〜50 rad/s |
| 0x7018 | limit_cur | float | 速度/位置モード電流制限 | 0〜11 A |
| 0x7022 | acc_rad | float | 速度モード加速度 | rad/s^2 |
| 0x701B | mechVel | float | 負荷側速度フィードバック | -50〜50 rad/s |
| 0x701C | VBUS | float | バス電圧 | V |

## 最小試験値

| 項目 | 初期値 |
|---|---:|
| motor_id | 実機設定値。未確認なら 1 から確認 |
| host/master_id | 0xFDなど固定値。実装時に統一 |
| limit_cur | 1.0 A |
| acc_rad | 2〜5 rad/s^2 |
| spd_ref | ±1 rad/s から開始 |

## Safety

- 制御モード切替は停止状態で行う。
- 電源投入直後はDinMeter側状態を `Disabled` とし、EDULITEへEnableを送らない。
- `Armed` 遷移時は target=0 を確認してから Enable。
- `Disabled` 遷移時は Type 4 Stop を送る。
- CAN timeout, bus-off, fault frame, feedback途絶で `Fault` へ遷移する。
- 20V運用では最高速不足の実測あり。速度要求が高い場合は24V以上を検討する。
- 回生・反力・手回し時の過電圧に注意する。

## Fault / Warning

主なFault情報:

| Bit | 内容 |
|---:|---|
| bit14 | gridlock / overload |
| bit7 | encoder not calibrated |
| bit3 | overvoltage |
| bit2 | undervoltage |
| bit1 | driver chip fault |
| bit0 | motor overtemperature |

## Codex向け実装メモ

推奨クラス名:

```cpp
class CanEduLitePrivateDriver : public MotorDriver {
public:
  bool begin(uint8_t motorId, uint16_t hostId);
  bool setVelocityMode(float currentLimitA, float accRadPerSec2);
  bool arm();
  bool disarm();
  bool setTargetPercent(int target); // -100..100 -> rad/s
  bool setVelocityRadPerSec(float rad_s);
  void update();
};
```

実装分割:

- `protocols/RobStridePrivateProtocol.*`: frame packing, float write/read, ID helpers
- `drivers/CanEduLitePrivateDriver.*`: state, safety, target mapping
- `transport/CanTransportTwai.*`: ESP32-S3 TWAI送受信

## 未確定事項

- 実機の初期CAN ID。
- Private protocol / MIT / CANopen のどれを長期標準にするか。
- 通信タイムアウトの実装値。
- 電源電圧を20V固定にするか、24V以上へ上げるか。

## Sources

- User-provided file: `EL05User Manual251210.pdf`
