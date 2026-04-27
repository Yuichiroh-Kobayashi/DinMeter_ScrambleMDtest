# Agent Skills

このディレクトリには、AIコーディングエージェント向けのタスク別手順を置く。

`AGENTS.md` は共通入口であり、`agent-skills/` は繰り返し作業の手順書である。

## Skills

| Skill | Use when |
|---|---|
| `firmware-bringup` | PlatformIO設定、起動確認、表示、Serialログを変更するとき |
| `motor-driver-profile` | MD20A、C620、EDULITE、DDT、AIR40AなどのドライバProfileを追加・変更するとき |
| `hardware-spec-author` | `docs/hardware/*/spec.md` を作成・更新するとき |
| `user-manual-writer` | 取扱説明書、Quick Start、Troubleshootingを作成・更新するとき |
| `safety-reviewer` | 実出力、状態遷移、安全仕様に関わる変更をレビューするとき |

Skillを使う前に、必ずルートの `AGENTS.md` を読む。
