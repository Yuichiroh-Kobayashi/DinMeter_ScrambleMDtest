/**
 * @file
 * @brief CytronMotorDriverを使うMD20A wrapper。
 */

#ifndef CYTRON_MD20A_DRIVER_H
#define CYTRON_MD20A_DRIVER_H

#include "DriverStatus.h"
#include "MotorDriver.h"
#include "config/FeatureFlags.h"
#include <Arduino.h>

#if ENABLE_REAL_MOTOR_OUTPUT
#include <CytronMotorDriver.h>
#endif

/**
 * @brief Cytron MD20AをCytron公式ライブラリ経由で扱うドライバ。
 *
 * このクラスはアプリケーション層からCytronMDを直接呼ばせないためのwrapperである。
 * `setTargetPercent()` は目標値の保存だけを行い、実出力はArmed状態の `update()` に限定する。
 */
class CytronMd20aDriver : public MotorDriver {
public:
  /**
   * @brief MD20A wrapperを生成する。
   *
   * constructorではCytronMDオブジェクトを生成しない。
   * CytronMDのconstructorはGPIOへ安全値を書き込むため、Armed中の出力関数で遅延生成する。
   *
   * @param pwmPin PWM入力に接続するGPIO番号。
   * @param dirPin DIR入力に接続するGPIO番号。
   */
  CytronMd20aDriver(int pwmPin, int dirPin);

  bool begin() override;
  bool arm() override;
  void disarm() override;
  void setTargetPercent(int targetPercent) override;
  void update() override;
  DriverStatus status() const override;

private:
  static constexpr int kTargetPercentMin = -100;
  static constexpr int kTargetPercentMax = 100;
  static constexpr int kCytronSpeedMin = -255;
  static constexpr int kCytronSpeedMax = 255;

  int clampTargetPercent(int targetPercent) const;
  int mapTargetPercentToSpeedCommand(int targetPercent) const;

  /**
   * @brief MD20Aへ停止指令を適用する。
   *
   * ENABLE_REAL_MOTOR_OUTPUTが1のときだけ `CytronMD::setSpeed(0)` を呼ぶ。
   */
  void applyZeroOutput();

  /**
   * @brief MD20Aへ速度指令を出力する。
   *
   * ENABLE_REAL_MOTOR_OUTPUTが1かつArmed状態のときだけ呼び出す。
   *
   * @param speedCommand CytronMotorDriverの速度指令。範囲は -255 から 255。
   */
  void writeSpeedOutput(int speedCommand);

#if ENABLE_REAL_MOTOR_OUTPUT
  CytronMD *motor();
#endif

  bool pinsAreConfigured() const;
  void logOutputPreviewIfChanged(int targetPercent, DriverStatus driverStatus);

  const int pwmPin_;
  const int dirPin_;
#if ENABLE_REAL_MOTOR_OUTPUT
  CytronMD *motor_;
#endif
  int targetPercent_;
  int lastLoggedTargetPercent_;
  DriverStatus status_;
  DriverStatus lastLoggedStatus_;
};

#endif // CYTRON_MD20A_DRIVER_H
