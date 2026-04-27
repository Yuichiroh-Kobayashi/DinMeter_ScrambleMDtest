# User Manual Policy

## Purpose

この文書は、AIエージェントがエンドユーザー向け取扱説明書を作成・更新するときの規約である。

対象AI:
- Codex
- Antigravity
- Claude Code

## Priority

取扱説明書は、以下の順で内容を決定する。

1. 法令・安全上の制約
2. AboutMe正典
3. このリポジトリの `AGENTS.md`
4. `docs/standards/safety_policy.md`
5. `docs/hardware/*/spec.md`
6. 実装済みコード
7. 外部情報

不明な仕様は推測で断定しない。`不明` または `未実装` と書く。

## Audience

読者は、Scrambleジュニアロボットチームの指導者・メンター・中高生メンバーである。

説明は、専門家向けの実装仕様ではなく、実際に装置を使う人向けに書く。

## Writing Rules

- 日本語で書く。
- 手順は番号付きリストにする。
- 1つの手順には1つの操作を中心に書く。
- 操作場所を先に書く。
- 例: 「DinMeterの画面で、StateがDisabledであることを確認します。」
- 危険、禁止、確認事項を曖昧にしない。
- 「推奨」ではなく、必要なものは「必ず」と書く。
- 未実装機能を使えるように書かない。
- 内部実装の詳細は、ユーザー操作に必要な範囲に限る。

## Safety Rules

以下は必ず記載する。

- 電源投入直後はDisabledであること。
- target = 0 のときだけ Armed にできること。
- Armedにする前に、配線、電源電圧、対象プロファイルを確認すること。
- Fault時は出力しないこと。
- モータや車輪に触れた状態でArmedにしないこと。
- 実験時は指導者またはメンターが立ち会うこと。

## Documentation Update Rule

コード変更でユーザー操作、表示、配線、対応デバイス、安全仕様が変わった場合、同じ変更で以下を更新する。

- `docs/manuals/user_manual.md`
- 必要に応じて `docs/manuals/quick_start.md`
- 必要に応じて `docs/manuals/troubleshooting.md`

## Prohibited Content

- 生徒の氏名、顔写真、個人情報を含めない。
- 校内写真を前提にしない。
- 未検証の安全性能を断定しない。
- 非常停止装置の代替になると書かない。
- 対応していないモータを対応済みと書かない。
