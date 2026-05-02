# DD Motor Safety Test Log

実モータ出力を伴うDDモータ（EDULITE 05, DDT-M0602C234等）の検証を行う際は、以下のテストゲートに従って確認を行い、結果を記録すること。

モータドライバ内蔵型のため、旧来の「モータ未接続での波形確認」は適用しづらい。代わりに「通信・Telemetry検証」を第一ゲートとする。

## Verification Gates

### Gate A: Communication & Telemetry Verification (モータ駆動なし)
**目的**: モータドライバへの電源は供給するが、**Enable(駆動許可)コマンドは絶対に送信しない**状態で、通信と状態遷移が安全に行われるか確認する。

- **Date**: YYYY-MM-DD
- **Target Device**: (e.g., RobStride EDULITE 05 / DDT-M0602C234)
- **Checklist**:
  - [ ] `Disabled` 状態で、モータが手で自由に回せる（フリー状態である）こと。
  - [ ] 軸を手で回した際、位置や速度のTelemetryが正しく受信できていること。
  - [ ] 通信ケーブル（CAN/RS485）を抜いた際、DinMeterが即座に通信タイムアウトを検知し `Fault` 状態へ遷移すること。
  - [ ] `Fault` から `Disabled` へ安全に復帰できること。
- **Result/Notes**:
  - (結果や気付きを記述)

---

### Gate B: Real Output Verification (低トルク駆動確認)
**目的**: 実際にEnableコマンドを送信し、低速・低トルクでの動作と安全停止を確認する。
**前提条件**: **Gate A がパスしていること。**

#### 1. No-load Bring-up (無負荷確認)
- **Date**: YYYY-MM-DD
- **Checklist**:
  - [ ] 電源投入直後（Power-on）のデフォルト状態が `Disabled` であり、モータが動かないこと。
  - [ ] `Armed` へ遷移し、`target=0` の状態で暴走しないこと。
  - [ ] 最小限の指令値を与えた際、意図した方向へ滑らかに回転すること。
  - [ ] `Armed` 動作中に `Disabled` へ遷移した瞬間、Enableが解除されモータがフリー（または停止）状態になること。
- **Result/Notes**:
  - (結果や気付きを記述)

#### 2. Loaded & Fault Verification (負荷・異常状態確認)
- **Date**: YYYY-MM-DD
- **Checklist**:
  - [ ] 小さな負荷（手やバネ）をかけた際、指令値に対してドライバから妥当なTelemetry（電流増加等）が返ってくること。
  - [ ] 動作中に意図的に通信ケーブルを抜いた際、ドライバ側が自律的に停止（フェイルセーフ）すること。
  - [ ] 連続動作時に異常な発熱や異音がないこと。
- **Result/Notes**:
  - (結果や気付きを記述)

---

## 注意事項

- 本ログは、新しいハードウェア（モータ）を追加・検証する際、または安全制御ロジックに変更を加えた際に必ず記録すること。
- 検証中はすぐに主電源を遮断できる物理的なキルスイッチ（電源スイッチ等）を手元に用意すること。
- **いかなる場合も、PWM値やdriver input currentをトルク・力とみなして検証結果を記述しないこと。**
