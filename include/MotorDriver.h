/**
 * @file
 * @brief モータドライバ共通インターフェース。
 */

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "DriverStatus.h"

/**
 * @brief モータドライバが実装する共通API。
 *
 * constructor、begin()、setTargetPercent()では物理出力を行わない。
 * 実出力が必要なプロファイルでも、出力はArmed状態のupdate()内に限定する。
 */
class MotorDriver {
public:
  virtual ~MotorDriver() = default;

  /**
   * @brief ドライバを初期化する。
   *
   * この関数では意図しない回転を避けるため、実モータ出力を開始しない。
   *
   * @return 初期化を継続できる場合はtrue。
   */
  virtual bool begin() = 0;

  /**
   * @brief targetが0であることを前提に、出力許可状態へ移る。
   *
   * @return Armedへ移行できた場合はtrue。
   */
  virtual bool arm() = 0;

  /**
   * @brief 出力停止状態へ戻し、物理出力をゼロまたは未接続相当にする。
   */
  virtual void disarm() = 0;

  /**
   * @brief 目標値を保存する。
   *
   * この関数はGPIO、PWM、CAN、RS485などへ直接出力しない。
   *
   * @param targetPercent -100から100までの目標値。
   */
  virtual void setTargetPercent(int targetPercent) = 0;

  /**
   * @brief ドライバ周期処理を行う。
   *
   * 物理出力を行う実装では、Armed状態のときだけ出力関数を呼ぶ。
   */
  virtual void update() = 0;

  /**
   * @brief 現在のドライバ状態を返す。
   *
   * @return ドライバ状態。
   */
  virtual DriverStatus status() const = 0;
};

#endif // MOTOR_DRIVER_H
