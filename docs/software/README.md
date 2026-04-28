# Software Dependencies

このディレクトリには、外部ライブラリやSDKに関するAI向け仕様を置く。

## Files

| Directory | Role |
|---|---|
| `CytronMotorDriver/` | Cytron公式モータドライバライブラリの使用方針 |

## Policy

- 公式ライブラリがある場合は、Reuse-firstの原則に従い、まず公式ライブラリを検討する。
- ただし、アプリ層や安全状態機械から公式ライブラリを直接呼ばない。
- 公式ライブラリは、このプロジェクトの `MotorDriver` interface に合わせたwrapper classから呼ぶ。
- 公式ライブラリの仕様が不明な場合は、推測せず `未検証` と記録する。
