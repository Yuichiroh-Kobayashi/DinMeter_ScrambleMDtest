# T-MOTOR AIR 40A AI Hardware Notes

このファイルは、AI/Codex が T-MOTOR AIR 40A を DinMeter_ScrambleMDtest で扱うための要約仕様です。実装時は、公式ページ、ESC実機、使用モータ、プロペラ/負荷なし試験を優先してください。

## 位置づけ

- T-MOTOR AIR 40A は、主にマルチローターUAV向けのセンサレスブラシレスESC。
- このプロジェクトでは、PWM系ESCとして「回すだけ」の対象に含める。
- **正逆転対応は公式情報から確認できないため、初期実装では Forward-only profile として扱う。**

## 主要仕様

| 項目 | 値 / 内容 |
|---|---|
| 種別 | Multi-rotor UAV / drone ESC |
| 電源 | 2〜6S |
| 電流 | 40Aクラス |
| 信号 | PWM系入力 |
| 最大信号周波数 | 621 Hz（公式ページ表記） |
| 正逆転 | 未確認。初期実装では不可として扱う |

## 制御方針

- AIR 40Aは一般的なESCとして、低パルス幅=停止/低スロットル、高パルス幅=高スロットルとして扱う。
- 公式ページから正逆転仕様は確認できないため、target < 0 は 0 に丸める。
- `Disabled` では停止パルスまたは信号停止。実機挙動を確認して決める。
- 安全のため、初回試験はプロペラ・危険負荷なしで行う。

## 初期プロファイル

| 項目 | 値 |
|---|---|
| profile name | AIR40A_PWM_FORWARD_ONLY |
| target入力 | 0〜100のみ有効 |
| target < 0 | 0へ丸める |
| PWM周波数 | 50〜400Hzから安全に試験。公式最大621Hz以下 |
| パルス幅 | 1000〜2000us系を仮定。ただし要実測/ESC校正 |

## DinMeter接続案

| ESC | DinMeter / Adapter | 備考 |
|---|---|---|
| Signal | PWM GPIO | 3.3V信号で認識するか要確認 |
| GND | GND | 信号GND共通 |
| Power | 外部バッテリ | DinMeterから供給しない |
| Motor 3相 | BLDC motor | 相線入替で回転方向が変わる可能性あり |

## Safety

- 電源投入時は必ず停止パルス、または信号なし。
- `Armed` に入る前に target=0。
- target < 0 は禁止。画面上に `Forward only` を表示する。
- 初回試験は無負荷または安全固定状態で行う。
- プロペラ、車輪、ベルトなど危険な外部負荷を接続した状態での初回試験は禁止。
- ESCキャリブレーション手順が必要な場合は、別ドキュメントに分ける。

## Codex向け実装メモ

推奨クラス名:

```cpp
class ServoPwmEscForwardOnlyDriver : public MotorDriver {
public:
  bool begin(int pwmPin);
  bool arm();
  bool disarm();
  bool setTargetPercent(int target); // negative values clamp to 0
  void update();
};
```

実装方針:

- `target <= 0`: min pulse / stop
- `target > 0`: map to throttle pulse
- negative target is not reverse; do not generate reverse command.

## 未確定事項

- 正逆転対応の有無。
- PWMパルス幅範囲。
- 起動時アーミング手順。
- 3.3V PWM信号で安定認識するか。
- ESCキャリブレーション要否。

## Sources

- T-MOTOR AIR 40A official product page: https://store.tmotor.com/jp/product/air-40a-6s-esc.html
