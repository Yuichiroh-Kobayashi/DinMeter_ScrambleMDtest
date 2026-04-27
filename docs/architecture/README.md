# Architecture

このディレクトリには、実装前に確認する設計資料を置く。

## Files

| File | Role |
|---|---|
| `safety_state_machine.md` | Disabled / Armed / Fault の状態遷移 |
| `driver_interface.md` | MotorDriver共通IF |
| `system_overview.md` | 将来追加予定。全体構成図 |
| `io_profile_matrix.md` | 将来追加予定。対象デバイスとI/O方式の対応表 |
| `protocol_abstraction.md` | 将来追加予定。CAN/RS485/PWMの分離方針 |

実装がこのディレクトリの設計と矛盾する場合、設計文書も同じ変更で更新する。
