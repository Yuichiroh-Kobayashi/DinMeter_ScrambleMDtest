# Cytron MD20A AI Hardware Notes

このファイルは、AI/Codex が Cytron MD20A を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、公式データシートと実機検証を優先してください。

## 位置づけ

- MD20A は、1chのブラシ付きDCモータ用Hブリッジドライバ。
- このプロジェクトでは、PWM + DIR の最初の実出力確認対象として扱う。
- CAN/RS485より単純で、DinMeterの安全状態機械の検証に適している。

## 主要仕様

| 項目 | 値 / 内容 |
|---|---|
| 対象モータ | Brushed DC motor x1 |
| モータ電圧 | DC 6〜30 V |
| 連続電流 | 20 A |
| ピーク電流 | 60 A |
| 制御入力 | PWM + DIR |
| 入力ロジック | 1.8 V / 3.3 V / 5 V / 12 V互換 |
| PWM周波数 | 最大20 kHz |
| 保護 | 過電流、温度、低電圧 |
| 注意 | 電源入力に逆接保護なし |

## 推奨制御方式

### Sign-magnitude PWM

| 信号 | 内容 |
|---|---|
| DIR | 回転方向 |
| PWM | 0〜100% 速度指令 |

- 最初はこの方式を採用する。
- `Disabled` では PWM=0 とする。
- `Armed` でのみ target 値を PWM duty に変換する。

### Locked-antiphase PWM

- MD20Aは対応可能だが、このプロジェクトの初期実装では使わない。
- 初学者・教室運用ではsign-magnitudeの方が説明しやすい。

## Library policy

MD20Aは、Cytron公式 `CytronMotorDriver` ライブラリの `PWM_DIR` 方式に対応する。
このプロジェクトでは、MD20A制御は自作の `digitalWrite()` / `analogWrite()` を直接使わず、可能な限り `CytronMotorDriver` をwrapper経由で使用する。

## Control mode

| Item | Value |
|---|---|
| Library | `CytronMotorDriver` |
| Library mode | `PWM_DIR` |
| Speed command | `setSpeed(-255..255)` |
| Direction | speed sign |
| Stop | `setSpeed(0)` |

## Pin Assignment

| MD20A | DinMeter Port | GPIO | Status | Notes |
|---|---|---:|---|---|
| PWM | PORT.B Yellow | G2 | Provisional | CytronMotorDriver経由。実機での出力波形確認は未検証 |
| DIR | PORT.B White | G1 | Provisional | CytronMotorDriver経由。実機での方向確認は未検証 |
| GND | PORT.B Black / system GND | - | Required | 外部電源GNDと共通化 |
| VM / Motor Power | 外部電源 | - | Required | DinMeterから供給しない |

- `ENABLE_REAL_MOTOR_OUTPUT=0` の既定状態では、GPIO出力を行わない。
- ただし、コード上の定数として `PWM=G2`, `DIR=G1` を置いてよい。
- 実出力を有効化する前に、Disabled中のPWM=0相当を確認する。

## Recommended wrapper

```cpp
class CytronMd20aDriver : public MotorDriver {
public:
  bool begin();
  void arm();
  void disarm();
  void setTargetPercent(int targetPercent);
  void update(DriverStatus status);
};
```

## Implementation rule

- `CytronMD(PWM_DIR, pwmPin, dirPin)` をwrapper内部で使う。
- `CytronMD` はconstructorでGPIOへ安全値を書き込むため、`ENABLE_REAL_MOTOR_OUTPUT=1` かつArmed中の初回出力直前に遅延生成する。
- `setTargetPercent()` ではtargetを保存するだけにする。
- `update(DriverStatus::Armed)` のときだけ `motor.setSpeed(speedCommand)` を呼ぶ。
- `Disabled` / `Fault` では `motor.setSpeed(0)` 相当とする。
- `ENABLE_REAL_MOTOR_OUTPUT=0` の既定状態では、`CytronMD` の実出力呼び出しを行わず、Serial previewに留める。

## Safety

- 電源投入直後はPWM=0。
- `Disabled` 中はDIRを保持してよいが、PWMは0固定。
- `target=0` ではPWM=0。
- 初回実機試験はモータ電源を低電圧・電流制限付きにする。
- 逆接保護がないため、電源投入前チェックリストを必須化する。
- MD20Aは高電流のDCモータドライバであり、乾電池レベルの初学者には危険である。初学者向け・乾電池向けには MAKER-DRIVE profileを優先する。

## 未確定事項

- PWM周波数の最終値。
- 実機側モータの突入電流・ストール電流。

## Sources

- Cytron MD20A product page: https://www.cytron.io/ampp-20amp-6v-30v-dc-motor-driver
