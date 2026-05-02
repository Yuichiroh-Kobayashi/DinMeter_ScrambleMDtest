# Safety State Machine

```mermaid
stateDiagram-v2
    [*] --> Disabled

    Disabled --> Armed: long press && target == 0 && no fault
    Armed --> Disabled: long press
    Armed --> Disabled: click
    Disabled --> Fault: detected fault
    Armed --> Fault: detected fault
    Fault --> Disabled: clear fault && target == 0

    Disabled: output = zero
    Armed: target=0で遷移 / 出力可否はbackend別gateに従う
    Fault: output = zero
```

> [!WARNING]
> `Armed` 状態の出力扱いはバックエンドによって異なる。
> 既存のPWM/GPIOバックエンドでは `Armed` 中のプロファイルコマンド出力が許可されるが、DDモータ等の初期bring-upにおいて、`Armed` は non-zero command 許可状態ではない。
> DDモータ制御では、明示的な操作による `OutputEnabled` または `Active` 状態を `Armed` の先に新設し、non-zero command はその状態でのみ許可すべきである。
