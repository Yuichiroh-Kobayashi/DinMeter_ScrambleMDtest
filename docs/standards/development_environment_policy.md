# Development Environment Policy

## Primary environment

本リポジトリの主開発環境は PlatformIO とする。

理由:

- M5DinMeter / ESP32-S3 / M5Unified / Cytron workaround の実機検証がPlatformIO上で進んでいる。
- `ENABLE_REAL_MOTOR_OUTPUT` などの安全flagをPlatformIO前提で運用している。
- DDモータのCAN/RS485通信、transport/protocol/driver分離に適している。

## Secondary compatibility target

Arduino CLI / Arduino IDE は副系統の互換性確認対象とする。

- 主開発をArduino CLIへ移行しない。
- Arduino互換を確認する場合は、別ディレクトリまたはexport手順で扱う。
- Arduino CLI用profileを作成する場合は、`sketch.yaml` でFQBN、platform version、library versionを固定する。
- Arduino IDEにコピーして動かすことを目標にする場合も、安全flagの既定値は必ず実出力無効にする。

## Rule

- PlatformIOで動くことを正本とする。
- Arduino CLIで動かない場合でも、直ちにPlatformIO実装を崩さない。
- 互換性問題は `docs/operations/arduino_cli_compatibility_log.md` に記録する。
