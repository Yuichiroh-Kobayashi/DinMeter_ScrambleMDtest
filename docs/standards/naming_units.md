
# Naming and Units Policy

## Purpose

この文書は、変数名・関数名・型名に物理量、単位、安全上の意味を明示するための規約である。

## Core Rule

名前だけで、対象・単位・安全上の意味が分かるようにする。

悪い例:

```cpp
int value;
float limit;
int speed;
```

良い例:

```cpp
int targetPercent;
float targetRadPerSec;
float currentLimitA;
uint32_t timeoutMs;
```

## Units in Names

物理量を表す変数には、可能な限り単位を含める。

| 単位           | suffix例       |
| -------------- | -------------- |
| percent        | `Percent`      |
| rad/s          | `RadPerSec`    |
| rpm            | `Rpm`          |
| A              | `A`            |
| V              | `V`            |
| ms             | `Ms`           |
| us             | `Us`           |
| Hz             | `Hz`           |
| count          | `Count`        |
| degree         | `Deg`          |
| duty percent   | `DutyPercent`  |
| pulse width us | `PulseWidthUs` |

例:

```cpp
float targetRadPerSec;
int16_t targetRpm;
float busVoltageV;
uint32_t feedbackTimeoutMs;
uint16_t servoPulseWidthUs;
int16_t c620CurrentCommandCount;
```

## State Names

状態名は `SystemState` に合わせる。

```cpp
SystemState::Disabled
SystemState::Armed
SystemState::Fault
```

表記揺れは禁止。

禁止:

```cpp
STOP
READY
ENABLE
ERROR
```

ただし、外部プロトコル由来の名称は、元仕様との対応が必要な場合のみ許可する。

## Function Names

### Safe / No-output Functions

物理出力を行わない関数名は、読み取り・変換・描画であることを明示する。

```cpp
updateDisplay()
readEncoderInput()
mapTargetPercentToRpm()
packC620CurrentFrame()
parseC620FeedbackFrame()
```

### Physical Output Functions

物理出力を行う関数名には、必ず副作用が分かる語を含める。

```cpp
writePwmOutput()
sendCanFrame()
sendRs485Frame()
applyMotorOutput()
forceZeroOutput()
```

曖昧な名前は禁止。

```cpp
update()
run()
process()
handle()
```

これらを使う場合は、クラス名やコメントで副作用範囲を明示する。

## Class Names

| 対象           | 推奨クラス名                   |
| -------------- | ------------------------------ |
| DinMeter本体   | `DinMeterBoard`                |
| 画面表示       | `ScreenView`                   |
| エンコーダ入力 | `EncoderInput`                 |
| 安全状態機械   | `SafetyStateMachine`           |
| CAN transport  | `CanTransportTwai`             |
| C620 driver    | `CanC620Driver`                |
| EDULITE driver | `CanEduLitePrivateDriver`      |
| DDT driver     | `Rs485DdtM0602Driver`          |
| MD20A driver   | `PwmDirDriverMd20a`            |
| AIR40A driver  | `ServoPwmEscForwardOnlyDriver` |

## Constants

定数は `constexpr` を優先する。

```cpp
constexpr int kTargetPercentMin = -100;
constexpr int kTargetPercentMax = 100;
constexpr uint32_t kSerialBaudRate = 115200;
```

ただし、Arduino IDE互換で問題が出る場合は `const` を許可する。

## Generic Names

以下は原則禁止。

```cpp
data
value
tmp
val
buf
flag
```

例外:

- スコープが10行未満
- プロトコル仕様上 `data[8]` のように一般名が妥当
- 直後に意味が明確な変換を行う

例:

```cpp
uint8_t data[8]; // CAN frame payload
```
