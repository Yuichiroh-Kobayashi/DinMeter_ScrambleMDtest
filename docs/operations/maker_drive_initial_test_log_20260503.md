# MAKER-DRIVE Initial Motor Test Log (2026-05-03)

## 1. 試験概要

- **試験日**: 2026-05-03
- **試験者**: USER (via Antigravity)
- **対象デバイス**: M5Stack DinMeter v1.1 + Cytron MAKER-DRIVE
- **目的**: 実モータ（小型DCモータ）を接続し、双方向の回転および速度制御が正常に行えるかを確認する。

## 2. 試験環境

- **モータ**: FA-130相当（減速機付き）
- **モータ電源**: アルカリ乾電池2本直列（実測 約3.0V）
- **負荷条件**: 減速機出力軸にて無負荷
- **接続**:
  - DinMeter PORT.B Yellow (G2) -> MAKER-DRIVE M1A
  - DinMeter PORT.B White (G1) -> MAKER-DRIVE M1B
  - DinMeter PORT.B Black (GND) -> MAKER-DRIVE GND
- **ソフトウェアバージョン**: `dev/bringup` ブランチ
- **FeatureFlags**:
  - `ENABLE_REAL_MOTOR_OUTPUT 1`
  - `DEFAULT_MOTOR_PROFILE_MAKER_DRIVE 1`

## 3. 確認項目と結果

| No | 状態 | 操作 / Target | 結果 | 備考 |
|---:|---|---:|---|---|
| 1 | Armed | 0 | 停止（無音・無振動） | 正常 |
| 2 | Armed | 10 -> 100 | 正転方向へ回転。Target増加に伴い回転数上昇を確認 | 正常 |
| 3 | Armed | -10 -> -100 | 逆転方向へ回転。Target減少に伴い回転数上昇を確認 | 正常 |
| 4 | Armed -> Disabled | (回転中) | 即座に停止 | 正常 |
| 5 | Fault時 | 任意 | 出力なし | 正常 |

## 4. 特記事項

- **ESP32-S3 LEDC問題の解決**: 当初の試験で `CytronMotorDriver` ライブラリの `ledcWrite` 実装が動作しない問題が発生したが、ドライバ内部で `ledcSetup` および `ledcAttachPin` を明示的に呼び出すことで解決した。
- **低電圧動作**: 3.0Vの低電圧電源においても、MAKER-DRIVEおよびモータが安定して動作することを確認した。
- **双方向性**: 正転・逆転ともに意図した通りのピンからPWMが出力され、モータが回転することを確認した。

## 5. 判定

**PASS**

MAKER-DRIVEを用いた小型DCモータの基本制御が成立したと判断する。  
次段階は、既知負荷・治具・電流制限条件を明確にしたうえで、低負荷試験計画を作成してから実施する。
