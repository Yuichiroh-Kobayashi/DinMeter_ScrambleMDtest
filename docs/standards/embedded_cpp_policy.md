# Embedded C++ / Arduino Compatibility Policy

## Purpose

この文書は、DinMeter_ScrambleMDtest のファームウェアを、PlatformIOを主開発環境としつつ、Arduino IDEへ移植しやすい形で保つための規約である。

## Target

- M5Stack DinMeter v1.1
- M5StampS3 / ESP32-S3
- PlatformIO + Arduino framework
- 将来のArduino IDE移植

## Basic Rule

主開発はPlatformIOで行う。  
ただし、コードはArduino IDEへ移植しやすいArduino-compatible C++として書く。

## Arduino IDE Compatibility

Arduino IDE互換のため、以下を守る。

- `setup()` と `loop()` を維持する。
- `main()` を自前で定義しない。
- PlatformIO専用APIへ直接依存しない。
- `src/main.cpp` は、将来 `.ino` 化しやすい構成にする。
- 共有ヘッダは `include/` に置くが、Arduino IDE移植時に同一sketchフォルダへコピーできる粒度にする。
- PlatformIO固有のビルドフラグに依存する処理は、`#ifdef` で隔離する。
- Arduino IDE用エクスポートが必要になったら、`extras/arduino_ide/` 以下に別途作成する。

## File Layout Policy

PlatformIO側の標準構成:

```text
src/
  main.cpp
include/
  AppState.h
```

Arduino IDEエクスポート時の候補:

```text
extras/arduino_ide/
  DinMeter_ScrambleMDtest/
    DinMeter_ScrambleMDtest.ino
    AppState.h
```

Arduino IDEでは、sketchフォルダ名と主 .ino ファイル名を一致させる。

## Pin Assignment Status Policy

GPIO割当には、次の4段階を使う。

| Status | Meaning | Code policy |
|---|---|---|
| Candidate | 候補。設計判断未確定 | `-1` または未使用を許可 |
| Provisional | 仮割当。コード定数にしてよい | 定数化してよい。ただし実出力は無効 |
| Verified | 実機確認済み | 実出力試験へ進んでよい |
| Reserved | 予約済みまたは使用禁止 | 使用しない |

AIエージェントは、`docs/architecture/io_profile_matrix.md` で `Provisional` または `Verified` とされたピンを、コード定数として実装してよい。
ただし、`ENABLE_REAL_MOTOR_OUTPUT=0` の既定状態では、`pinMode`, `digitalWrite`, `ledcWrite`, CAN送信, RS485送信などの物理出力を行ってはならない。

## Initialization Order

`setup()` では次の順番を守る。

- 安全な出力初期値を設定する。
- DinMeterの電源保持 `HOLD(GPIO46)` を設定する。
- M5/DinMeterライブラリを初期化する。
- Serialログを開始する。
- 画面表示を初期化する。
- 状態を `Disabled` 、targetを `0` として表示する。

## Motor Output Rule

- `begin()` は物理出力を出してはならない。
- constructor は物理出力を出してはならない。
- 実モータ出力は Armed 状態の明示的な `update()` 内だけで行う。
- `Disabled` と `Fault` では、出力はゼロ、停止パルス、未接続、またはHigh-Z相当にする。
- C620など閉ループ制御対象では、`Disabled` 中に速度0制御を回して制動電流を出してはならない。

## GPIO / Bus Policy

- GPIO番号は直接散らばらせず、定数名で管理する。
- 物理出力を伴う関数名には `write`, `send`, `apply`, `output` のいずれかを含める。
- CAN、RS485、PWM、PWM/DIRは、UIや状態機械から直接呼ばない。
- プロトコルpack/unpackと物理I/Oは別ファイルに分ける。

## Timing Policy

- `delay()` はBring-up段階のみ許可する。
- 実モータ出力段階では、周期処理は `millis()` ベースへ移行する。
- 通信timeout値は定数化し、単位を変数名に含める。

## Logging Policy

Serialログには次を含める。

- 起動
- state遷移
- target変更
- profile変更
- fault発生
- 通信timeout
- bus-offなどの通信異常

ログに個人情報、校内情報、学校内ネットワーク情報を含めない。

## External Library Reuse Policy

公式ライブラリが存在するデバイスは、Reuse-firstの原則に従い、まず公式ライブラリの使用を検討する。

ただし、アプリケーション層やSafetyStateMachineから公式ライブラリを直接呼んではならない。

外部ライブラリは、必ずproject wrapper classで包む。

例:

```text
App / SafetyStateMachine
  -> MotorDriver interface
  -> CytronMd20aDriver
  -> CytronMotorDriver library
```

wrapper classは、次を保証する。

- `begin()` で意図しない物理出力を出さない。
- `setTargetPercent()` は値の保存だけにする。
- `update()` の `Armed` 状態でのみ外部ライブラリの出力関数を呼ぶ。
- `Disabled` / `Fault` では停止指令または出力なしにする。
- `ENABLE_REAL_MOTOR_OUTPUT=0` の既定状態では外部ライブラリの出力関数を呼ばない。
