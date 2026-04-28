/**
 * @file
 * @brief CytronMotorDriverを使うMAKER-DRIVE wrapper。
 */

#ifndef CYTRON_MAKER_DRIVE_DRIVER_H
#define CYTRON_MAKER_DRIVE_DRIVER_H

#include "DriverStatus.h"
#include "MotorDriver.h"
#include "config/FeatureFlags.h"
#include <Arduino.h>

#if ENABLE_REAL_MOTOR_OUTPUT
#include <CytronMotorDriver.h>
#endif

/**
 * @brief Cytron MAKER-DRIVEをCytron公式ライブラリ経由で扱うドライバ。
 *
 * 初期実装ではM1A/M1Bを使うsingle motor modeを標準とする。
 * M2A/M2Bを使うdual motor modeの構造は持つが、UIからの選択や操作は未実装。
 */
class CytronMakerDriveDriver : public MotorDriver {
public:
  /**
   * @brief MAKER-DRIVE wrapperを生成する。
   *
   * constructorではCytronMDオブジェクトを生成しない。
   * 実出力有効時はbegin()で生成し、停止値だけを明示する。
   *
   * @param m1aPin M1A入力に接続するGPIO番号。
   * @param m1bPin M1B入力に接続するGPIO番号。
   * @param m2aPin M2A入力に接続するGPIO番号。
   * @param m2bPin M2B入力に接続するGPIO番号。
   * @param dualMotorMode trueならM2も同時に制御する。UIからの切替は未実装。
   */
  CytronMakerDriveDriver(int m1aPin, int m1bPin, int m2aPin, int m2bPin,
                         bool dualMotorMode = false);

  bool begin() override;
  bool arm() override;
  void disarm() override;
  void setTargetPercent(int targetPercent) override;
  void update() override;
  DriverStatus status() const override;

  /**
   * @brief 2ch mode用の左右目標値を保存する。
   *
   * この関数は値の保存だけを行い、`setSpeed()` は呼ばない。
   * 現時点ではUIから呼び出す経路は未実装。
   *
   * @param motor1TargetPercent M1の目標値。範囲は -100 から 100。
   * @param motor2TargetPercent M2の目標値。範囲は -100 から 100。
   */
  void setDualTargetPercent(int motor1TargetPercent, int motor2TargetPercent);

private:
  static constexpr int kTargetPercentMin = -100;
  static constexpr int kTargetPercentMax = 100;
  static constexpr int kCytronSpeedMin = -255;
  static constexpr int kCytronSpeedMax = 255;

  int clampTargetPercent(int targetPercent) const;
  int mapTargetPercentToSpeedCommand(int targetPercent) const;

  /**
   * @brief MAKER-DRIVEへ停止指令を適用する。
   *
   * ENABLE_REAL_MOTOR_OUTPUTが1のときだけ、生成済みのCytronMDへ `setSpeed(0)` を呼ぶ。
   */
  void applyZeroOutput();

  /**
   * @brief MAKER-DRIVEへ速度指令を出力する。
   *
   * ENABLE_REAL_MOTOR_OUTPUTが1かつArmed状態のときだけ呼び出す。
   *
   * @param motor1SpeedCommand M1の速度指令。範囲は -255 から 255。
   * @param motor2SpeedCommand M2の速度指令。範囲は -255 から 255。
   */
  void writeSpeedOutput(int motor1SpeedCommand, int motor2SpeedCommand);

#if ENABLE_REAL_MOTOR_OUTPUT
  CytronMD *motor1();
  CytronMD *motor2();
#endif

  bool pinsAreConfigured() const;
  bool speedCommandIsValid(int speedCommand) const;
  void logOutputPreviewIfChanged(int motor1TargetPercent,
                                 int motor2TargetPercent,
                                 DriverStatus driverStatus);

  const int m1aPin_;
  const int m1bPin_;
  const int m2aPin_;
  const int m2bPin_;
  const bool dualMotorMode_;
#if ENABLE_REAL_MOTOR_OUTPUT
  CytronMD *motor1_;
  CytronMD *motor2_;
#endif
  int motor1TargetPercent_;
  int motor2TargetPercent_;
  int lastLoggedMotor1TargetPercent_;
  int lastLoggedMotor2TargetPercent_;
  DriverStatus status_;
  DriverStatus lastLoggedStatus_;
};

#endif // CYTRON_MAKER_DRIVE_DRIVER_H
