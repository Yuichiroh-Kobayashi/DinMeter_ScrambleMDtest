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

## DinMeter接続案

| MD20A | DinMeter候補 | 備考 |
|---|---|---|
| PWM | PORT.B G2 など | LEDC PWM出力 |
| DIR | PORT.B G1 など | GPIO出力 |
| GND | DinMeter GND | 外部電源GNDと共通化 |
| Motor Power | 外部電源 | DinMeter 5Vから供給しない |

## 安全仕様

- 電源投入直後はPWM=0。
- `Disabled` 中はDIRを保持してよいが、PWMは0固定。
- `target=0` ではPWM=0。
- 初回実機試験はモータ電源を低電圧・電流制限付きにする。
- 逆接保護がないため、電源投入前チェックリストを必須化する。

## Codex向け実装メモ

推奨クラス名:

```cpp
class PwmDirDriverMD20A : public MotorDriver {
public:
  bool begin(int pwmPin, int dirPin);
  void arm();
  void disarm();
  void setTargetPercent(int target); // -100..100
  void update();
};
```

実装方針:

- `target > 0`: DIR=forward, PWM=abs(target)
- `target < 0`: DIR=reverse, PWM=abs(target)
- `target == 0` or Disabled: PWM=0
- PWM周波数は最初 1 kHz〜20 kHz の範囲で安全に設定する。騒音・発熱・挙動を見て調整する。

## 未確定事項

- DinMeterでPWM/DIRに使う最終GPIO。
- PWM周波数の最終値。
- 実機側モータの突入電流・ストール電流。

## Sources

- Cytron MD20A product page: https://www.cytron.io/ampp-20amp-6v-30v-dc-motor-driver
