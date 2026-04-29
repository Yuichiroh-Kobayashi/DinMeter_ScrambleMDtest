
# Cytron MAKER-DRIVE AI Hardware Notes

このファイルは、AI/Codex が Cytron MAKER-DRIVE を DinMeter_ScrambleMDtest で扱うための要約仕様である。実装時は公式データシート、公式ページ、実機検証を優先する。

## Position

- MAKER-DRIVE は、初学者向けの2ch H-Bridge DCモータドライバ。
- 乾電池で動く小型DCモータ教材向けに扱う。
- MD20Aより低電圧・低電流で、初回の教育用デモや安全なBring-upに向く。
- このプロジェクトでは `CytronMotorDriver` ライブラリの `PWM_PWM` 方式で扱う。

## Main specifications

| Item | Value |
|---|---|
| Motor type | Brushed DC motor |
| Channel | 2 |
| Motor voltage | 2.5〜9.5 VDC |
| Continuous current | 1 A per channel |
| Peak current | 1.5 A per channel, < 5 s |
| Logic input High | 1.7〜6 V |
| Logic input Low | 0〜0.5 V |
| PWM frequency | DC〜20 kHz |
| 5V output | Max 200 mA |
| Protection | Reverse polarity, overtemperature |

## Inputs

| Signal | Meaning |
|---|---|
| M1A | PWM input A for motor 1 |
| M1B | PWM input B for motor 1 |
| M2A | PWM input A for motor 2 |
| M2B | PWM input B for motor 2 |

## Truth table

| Input A | Input B | Motor state |
|---|---|---|
| Low | Low | Brake |
| High | Low | Forward |
| Low | High | Backward |
| High | High | Coast |

実際のForward/Backward方向はモータ配線に依存する。

## Initial PWM safety policy

初期bring-upにおける安全設計として、出力パターンを以下に制限する。

- **Disabled / Fault / target=0**: Low / Low (Brake) とする。
- **target > 0**: PWM / Low とする。
- **target < 0**: Low / PWM とする。

初期bring-upでは、予期せぬ挙動を防ぐため、以下の状態は使用しない。
- High / High (Coast)
- PWM / High
- High / PWM

## Library policy

MAKER-DRIVE は、Cytron公式 `CytronMotorDriver` ライブラリの `PWM_PWM` 方式で制御する。

```cpp
CytronMD motor1(PWM_PWM, m1aPin, m1bPin);
CytronMD motor2(PWM_PWM, m2aPin, m2bPin);
```

`setSpeed(-255..255)` で正逆転と速度を指令する。

## Provisional Pin Assignment

2ch使用を想定し、DinMeterの外部GPIO 4本をすべて使う。

| MAKER-DRIVE | DinMeter Port               | GPIO | Status      | Notes           |
| ----------- | --------------------------- | ---: | ----------- | --------------- |
| M1A         | PORT.B Yellow               |   G2 | Provisional | Motor 1 A       |
| M1B         | PORT.B White                |   G1 | Provisional | Motor 1 B       |
| M2A         | PORT.A Yellow               |  G13 | Provisional | Motor 2 A       |
| M2B         | PORT.A White                |  G15 | Provisional | Motor 2 B       |
| GND         | PORT.A/B Black / system GND |    - | Required    | 外部電源GNDとの扱いを確認  |
| VB+ / VB-   | 外部乾電池など              |    - | Required    | DinMeterから供給しない |

## Recommended wrapper

```cpp
class CytronMakerDriveDriver : public MotorDriver {
public:
  bool begin();
  void arm();
  void disarm();
  void setTargetPercent(int targetPercent);
  void setDifferentialTargetPercent(int leftPercent, int rightPercent);
  void update(DriverStatus status);
};
```

## Initial implementation scope

### Option A: single motor mode (Default)

- M1A/M1Bのみ使用。既定の動作モードとする。
- `targetPercent` 1つで1個のDCモータを制御。
- 初回Bring-upに向く。
- `CytronMakerDriveDriver` の初期実装対象。

### Option B: dual motor mode

- M1A/M1B, M2A/M2Bを使用。
- 左右同一速度、または簡易差動制御に対応。
- UI未実装、別レビューが必要、実機未検証のため、初期実装では使用しない。
- `CytronMakerDriveDriver` に構造はあるが、UIから使う機能は未実装。

## Implementation status

| Item | Status |
|---|---|
| Wrapper class | `CytronMakerDriveDriver` |
| Default mode | Single motor mode |
| M1 output | 実装済み。ただし `ENABLE_REAL_MOTOR_OUTPUT=0` 既定ではSerial previewのみ |
| M2 output | 構造のみ。UI未実装、実機未検証 |
| Real output | 未検証 |

`ENABLE_REAL_MOTOR_OUTPUT=1` のとき、wrapperは `begin()` で `CytronMD(PWM_PWM, pinA, pinB)` を生成し、停止値 `setSpeed(0)` だけを明示する。非ゼロ出力はArmed中の `update()` に限定する。

## Safety

- 初回試験は乾電池または電流制限付き電源を使う。
- 小型DCモータのみを接続する。
- 1A連続を超えるモータは使わない。
- Motor test buttonは不用意に押すと全速回転するため、運用時は注意する。
- 5V出力は最大200mAであり、DinMeter本体電源として使う場合は電流余裕を検証する。
- DinMeterからモータ電源を供給しない。

## Unverified

- DinMeter 3.3V GPIOでの安定動作。
- `CytronMotorDriver` + ESP32-S3 + MAKER-DRIVE 実機動作。
- 2ch同時PWM時のLEDC channel割当。
- 5V出力からDinMeterへ給電する運用の可否。
