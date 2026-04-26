# Safety State Machine

## States

- `Disabled`: 起動直後の既定状態。実モータ出力は未実装。
- `Armed`: 今回は画面上の状態遷移のみ。実モータ出力は未実装。
- `Fault`: 将来の異常停止用。今回の最小実装では自動遷移なし。

## Initial conditions

- `state = Disabled`
- `target = 0`
- CAN、RS485、PWM/DIR、ESC出力は実装しない

## Transitions

- `Disabled -> Armed`
  - エンコーダボタン長押し
  - `target == 0`
- `Armed -> Disabled`
  - エンコーダボタン長押し
- `* -> Fault`
  - 未実装

## Output policy

Bring-up段階では、状態に関係なく実モータ出力を行わない。今後ドライバ実装を追加する場合も、`Disabled` と `Fault` では出力ゼロまたは未接続相当に固定する。
