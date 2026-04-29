# MAKER-DRIVE State Model

この文書は、MAKER-DRIVE (PWM_PWM モード) における、安全状態遷移(SafetyStateMachine)とドライバ入力ピン状態の対応を定義する。

## Default Mode: Single Motor Mode

既定の動作モードは **single motor mode** (M1A/M1Bのみ使用) とする。

| State | target | M1A (PWM) | M1B (PWM) | Description |
|---|---|---|---|---|
| Disabled / Fault | Any | Low | Low | Brake (停止状態の強制) |
| Armed | 0 | Low | Low | Brake |
| Armed | > 0 | PWM | Low | Forward (Initial PWM safety policy) |
| Armed | < 0 | Low | PWM | Backward (Initial PWM safety policy) |

※ 実際の回転方向はモータの配線に依存する。
※ Initial PWM safety policy に従い、High/High(Coast)、PWM/High、High/PWMは使用しない。

## Dual Motor Mode (Unused)

M1A/M1B および M2A/M2B の両方を使用するモード。
現在の実装では、**別レビューが完了するまで未使用(UI未実装・実機未検証)** とする。

将来的に有効化された場合の理論上の状態モデル(参考):

| State | M1 Target | M2 Target | M1A | M1B | M2A | M2B | Description |
|---|---|---|---|---|---|---|---|
| Disabled / Fault | Any | Any | Low | Low | Low | Low | All Brake |
| Armed | 0 | 0 | Low | Low | Low | Low | All Brake |
| Armed | > 0 | > 0 | PWM | Low | PWM | Low | All Forward |
| Armed | < 0 | > 0 | Low | PWM | PWM | Low | M1 Back, M2 Fwd |
