# CytronMotorDriver Library AI Notes

## Position

CytronMotorDriver は、Cytron公式のArduino用モータドライバライブラリである。

このプロジェクトでは、Cytron製DCモータドライバを制御するとき、可能な限りこの公式ライブラリを使う。

## Source

- GitHub: `CytronTechnologies/CytronMotorDriver`
- Arduino Library Manager name: `Cytron Motor Drivers Library`

## Supported profiles used in this project

| Project target | Cytron library mode | Notes |
|---|---|---|
| MD20A | `PWM_DIR` | PWMで速度、DIRで方向を制御 |
| MAKER-DRIVE | `PWM_PWM` | 入力A/Bの2PWMで正逆転を制御 |

## Installation

### Arduino IDE

Arduino IDE の Library Manager で `Cytron Motor Drivers Library` を検索してインストールする。

### PlatformIO

このプロジェクトでは、まずGitHub repoを `lib_deps` に指定する。

```ini
lib_deps =
  m5stack/M5DinMeter
  m5stack/M5Unified
  m5stack/M5GFX
  https://github.com/CytronTechnologies/CytronMotorDriver.git
```

ライブラリ名で解決できる場合は、後でPlatformIO Registry名に置き換えてよい。

## Usage pattern

### PWM_DIR / MD20A etc

```cpp
#include <CytronMotorDriver.h>

CytronMD motor(PWM_DIR, pwmPin, dirPin);

motor.setSpeed(0);     // stop
motor.setSpeed(128);   // forward
motor.setSpeed(-128);  // reverse
```

### PWM_PWM / MAKER-DRIVE etc

```cpp
#include <CytronMotorDriver.h>

CytronMD motor1(PWM_PWM, m1aPin, m1bPin);
CytronMD motor2(PWM_PWM, m2aPin, m2bPin);

motor1.setSpeed(0);
motor2.setSpeed(0);
```

## Project wrapper rule

アプリケーションコードから `CytronMD` を直接呼ばない。

必ず以下のwrapper classを経由する。

| Wrapper                  | Target      |
| ------------------------ | ----------- |
| `CytronMd20aDriver`      | MD20A       |
| `CytronMakerDriveDriver` | MAKER-DRIVE |

## Safety rules

- constructorで実モータ出力を出さない。
- `begin()` で意図しない回転を発生させない。
- `setTargetPercent()` は値の保存だけにする。
- `update()` の `Armed` 状態でのみ `setSpeed()` を呼ぶ。
- `Disabled` / `Fault` では `setSpeed(0)` 相当へ落とす。
- 既定では `ENABLE_REAL_MOTOR_OUTPUT=0` とし、物理出力を行わない。

## ESP32-S3 notes

- Arduino-ESP32の `analogWrite()` は0〜255範囲のPWM出力である。
- 必要に応じて `analogWriteFrequency(pin, frequencyHz)` で周波数を指定する。
- Cytronライブラリが内部で `analogWrite()` を使う前提のため、ESP32-S3実機で波形確認する。
- `CytronMD` のconstructorは `pinMode()` と `digitalWrite(LOW)` を行う。`ENABLE_REAL_MOTOR_OUTPUT=0` の既定状態ではconstructorも呼ばない。
- `ENABLE_REAL_MOTOR_OUTPUT=1` のときは、project wrapperの `begin()` で `CytronMD` を生成し、停止値 `setSpeed(0)` だけを明示する。

## Unverified

- ESP32-S3 + M5DinMeter + CytronMotorDriver library の実機動作。
- MD20Aでの最適PWM周波数。
- MAKER-DRIVEでの3.3V信号認識。データシート上はHigh 1.7〜6Vなので成立見込みだが実機確認する。
