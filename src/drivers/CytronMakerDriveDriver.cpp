/**
 * @file
 * @brief CytronMotorDriverを使うMAKER-DRIVE wrapper実装。
 */

#include "drivers/CytronMakerDriveDriver.h"

CytronMakerDriveDriver::CytronMakerDriveDriver(int m1aPin, int m1bPin,
                                               int m2aPin, int m2bPin,
                                               bool dualMotorMode)
    : m1aPin_(m1aPin), m1bPin_(m1bPin), m2aPin_(m2aPin), m2bPin_(m2bPin),
      dualMotorMode_(dualMotorMode),
#if ENABLE_REAL_MOTOR_OUTPUT
      motor1_(nullptr), motor2_(nullptr),
#endif
      motor1TargetPercent_(0), motor2TargetPercent_(0),
      lastLoggedMotor1TargetPercent_(101), lastLoggedMotor2TargetPercent_(101),
      status_(DriverStatus::Disabled), lastLoggedStatus_(DriverStatus::Fault) {
}

bool CytronMakerDriveDriver::begin() {
  motor1TargetPercent_ = 0;
  motor2TargetPercent_ = 0;
  status_ = DriverStatus::Disabled;

  if (!pinsAreConfigured()) {
    status_ = DriverStatus::Fault;
    Serial.println("MAKER-DRIVE Fault: PWM GPIO is invalid.");
    return false;
  }

#if ENABLE_REAL_MOTOR_OUTPUT
  motor1()->setSpeed(0);
  if (dualMotorMode_) {
    motor2()->setSpeed(0);
  }
#else
  Serial.println("MAKER-DRIVE: CytronMotorDriver output disabled by ENABLE_REAL_MOTOR_OUTPUT=0.");
#endif

  return true;
}

bool CytronMakerDriveDriver::arm() {
  if (motor1TargetPercent_ != 0 || motor2TargetPercent_ != 0) {
    Serial.println("MAKER-DRIVE Safety: target must be 0 before arm.");
    return false;
  }

  if (status_ == DriverStatus::Fault) {
    Serial.println("MAKER-DRIVE Safety: cannot arm while driver is Fault.");
    return false;
  }

  status_ = DriverStatus::Armed;
  return true;
}

void CytronMakerDriveDriver::disarm() {
  if (status_ != DriverStatus::Fault) {
    status_ = DriverStatus::Disabled;
  }
  // Fault時でも安全のため停止出力を試みる。
  // ただしFaultの原因(status_)はここでは解除せず、明示的な復帰処理を待つ。
  applyZeroOutput();
}

void CytronMakerDriveDriver::setTargetPercent(int targetPercent) {
  if (status_ == DriverStatus::Fault) {
    motor1TargetPercent_ = 0;
    motor2TargetPercent_ = 0;
    return;
  }
  const int safeTargetPercent = clampTargetPercent(targetPercent);
  motor1TargetPercent_ = safeTargetPercent;
  if (!dualMotorMode_) {
    motor2TargetPercent_ = 0;
  }
}

void CytronMakerDriveDriver::setDualTargetPercent(int motor1TargetPercent,
                                                  int motor2TargetPercent) {
  if (status_ == DriverStatus::Fault) {
    motor1TargetPercent_ = 0;
    motor2TargetPercent_ = 0;
    return;
  }
  motor1TargetPercent_ = clampTargetPercent(motor1TargetPercent);
  motor2TargetPercent_ = clampTargetPercent(motor2TargetPercent);
}

void CytronMakerDriveDriver::update() {
  if (status_ != DriverStatus::Armed) {
    applyZeroOutput();
    logOutputPreviewIfChanged(0, 0, status_);
    return;
  }

  const int motor1SpeedCommand =
      mapTargetPercentToSpeedCommand(motor1TargetPercent_);
  const int motor2SpeedCommand =
      dualMotorMode_ ? mapTargetPercentToSpeedCommand(motor2TargetPercent_) : 0;

#if ENABLE_REAL_MOTOR_OUTPUT
  writeSpeedOutput(motor1SpeedCommand, motor2SpeedCommand);
#else
  logOutputPreviewIfChanged(motor1TargetPercent_,
                            dualMotorMode_ ? motor2TargetPercent_ : 0,
                            status_);
#endif
}

DriverStatus CytronMakerDriveDriver::status() const { return status_; }

int CytronMakerDriveDriver::clampTargetPercent(int targetPercent) const {
  if (targetPercent < kTargetPercentMin) {
    return kTargetPercentMin;
  }
  if (targetPercent > kTargetPercentMax) {
    return kTargetPercentMax;
  }
  return targetPercent;
}

int CytronMakerDriveDriver::mapTargetPercentToSpeedCommand(
    int targetPercent) const {
  const int safeTargetPercent = clampTargetPercent(targetPercent);
  return (safeTargetPercent * kCytronSpeedMax) / kTargetPercentMax;
}

void CytronMakerDriveDriver::applyZeroOutput() {
#if ENABLE_REAL_MOTOR_OUTPUT
  if (motor1_ != nullptr) {
    motor1_->setSpeed(0);
  }
  if (motor2_ != nullptr) {
    motor2_->setSpeed(0);
  }
#endif
}

void CytronMakerDriveDriver::writeSpeedOutput(int motor1SpeedCommand,
                                              int motor2SpeedCommand) {
#if ENABLE_REAL_MOTOR_OUTPUT
  if (!speedCommandIsValid(motor1SpeedCommand) ||
      !speedCommandIsValid(motor2SpeedCommand)) {
    status_ = DriverStatus::Fault;
    motor1TargetPercent_ = 0; // Fault時はターゲットを即座にリセットする
    motor2TargetPercent_ = 0;
    applyZeroOutput();
    Serial.println("MAKER-DRIVE Fault: speed command is out of range.");
    return;
  }

  motor1()->setSpeed(motor1SpeedCommand);
  if (dualMotorMode_) {
    motor2()->setSpeed(motor2SpeedCommand);
  } else if (motor2_ != nullptr) {
    motor2_->setSpeed(0);
  }

  // TODO: 安全設計 (VAMeter-Edu由来)
  // 急激な方向反転(正転<->逆転)による過電流を防ぐため、
  // 直前の出力符号と現在の出力符号が異なる場合は、
  // 一度 setSpeed(0) を出力し、数msのdelayまたは次周期まで待つ処理を検討する。
  // 現在は即時切り替わるため、負荷の大きいモータでは電源リセットを誘発する恐れがある。
#else
  (void)motor1SpeedCommand;
  (void)motor2SpeedCommand;
#endif
}

#if ENABLE_REAL_MOTOR_OUTPUT
CytronMD *CytronMakerDriveDriver::motor1() {
  if (motor1_ == nullptr) {
    // 実出力有効時だけ生成し、begin()では停止値だけを明示する。
    motor1_ = new CytronMD(PWM_PWM, m1aPin_, m1bPin_);
  }
  return motor1_;
}

CytronMD *CytronMakerDriveDriver::motor2() {
  if (motor2_ == nullptr) {
    // 2ch modeはUI未実装のため、dualMotorModeが有効な場合だけ生成する。
    motor2_ = new CytronMD(PWM_PWM, m2aPin_, m2bPin_);
  }
  return motor2_;
}
#endif

bool CytronMakerDriveDriver::pinsAreConfigured() const {
  if (m1aPin_ < 0 || m1bPin_ < 0) {
    return false;
  }
  if (dualMotorMode_ && (m2aPin_ < 0 || m2bPin_ < 0)) {
    return false;
  }
  return true;
}

bool CytronMakerDriveDriver::speedCommandIsValid(int speedCommand) const {
  return speedCommand >= kCytronSpeedMin && speedCommand <= kCytronSpeedMax;
}

void CytronMakerDriveDriver::logOutputPreviewIfChanged(
    int motor1TargetPercent, int motor2TargetPercent,
    DriverStatus driverStatus) {
  if (motor1TargetPercent == lastLoggedMotor1TargetPercent_ &&
      motor2TargetPercent == lastLoggedMotor2TargetPercent_ &&
      driverStatus == lastLoggedStatus_) {
    return;
  }

  lastLoggedMotor1TargetPercent_ = motor1TargetPercent;
  lastLoggedMotor2TargetPercent_ = motor2TargetPercent;
  lastLoggedStatus_ = driverStatus;

  const int motor1SpeedCommand =
      mapTargetPercentToSpeedCommand(motor1TargetPercent);
  const int motor2SpeedCommand =
      mapTargetPercentToSpeedCommand(motor2TargetPercent);

  Serial.printf("MAKER-DRIVE Cytron preview: status=%d mode=%s m1Target=%d m1Speed=%d m2Target=%d m2Speed=%d output=%s\n",
                static_cast<int>(driverStatus),
                dualMotorMode_ ? "dual" : "single", motor1TargetPercent,
                motor1SpeedCommand, motor2TargetPercent, motor2SpeedCommand,
#if ENABLE_REAL_MOTOR_OUTPUT
                "real"
#else
                "log-only"
#endif
  );
}
