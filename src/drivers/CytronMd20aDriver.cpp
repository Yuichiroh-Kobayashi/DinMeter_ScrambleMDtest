/**
 * @file
 * @brief CytronMotorDriverを使うMD20A wrapper実装。
 */

#include "drivers/CytronMd20aDriver.h"
#include "config/FeatureFlags.h"

CytronMd20aDriver::CytronMd20aDriver(int pwmPin, int dirPin)
    : pwmPin_(pwmPin), dirPin_(dirPin),
#if ENABLE_REAL_MOTOR_OUTPUT
      motor_(nullptr),
#endif
      targetPercent_(0), lastLoggedTargetPercent_(101),
      status_(DriverStatus::Disabled), lastLoggedStatus_(DriverStatus::Fault) {
}

bool CytronMd20aDriver::begin() {
  targetPercent_ = 0;
  status_ = DriverStatus::Disabled;

  if (!pinsAreConfigured()) {
    status_ = DriverStatus::Fault;
    Serial.println("MD20A Fault: PWM/DIR GPIO is invalid.");
    return false;
  }

#if ENABLE_REAL_MOTOR_OUTPUT
  motor()->setSpeed(0);
#else
  Serial.println("MD20A: CytronMotorDriver output disabled by ENABLE_REAL_MOTOR_OUTPUT=0.");
#endif

  return true;
}

bool CytronMd20aDriver::arm() {
  if (targetPercent_ != 0) {
    Serial.println("MD20A Safety: target must be 0 before arm.");
    return false;
  }

  if (status_ == DriverStatus::Fault) {
    Serial.println("MD20A Safety: cannot arm while driver is Fault.");
    return false;
  }

  status_ = DriverStatus::Armed;
  return true;
}

void CytronMd20aDriver::disarm() {
  if (status_ != DriverStatus::Fault) {
    status_ = DriverStatus::Disabled;
  }
  // Fault時でも安全のため停止出力を試みる。
  // ただしFaultの原因(status_)はここでは解除せず、明示的な復帰処理を待つ。
  applyZeroOutput();
}

void CytronMd20aDriver::setTargetPercent(int targetPercent) {
  if (status_ == DriverStatus::Fault) {
    targetPercent_ = 0;
    return;
  }
  targetPercent_ = clampTargetPercent(targetPercent);
}

void CytronMd20aDriver::update() {
  if (status_ != DriverStatus::Armed) {
    applyZeroOutput();
    logOutputPreviewIfChanged(0, status_);
    return;
  }

  const int speedCommand = mapTargetPercentToSpeedCommand(targetPercent_);

#if ENABLE_REAL_MOTOR_OUTPUT
  writeSpeedOutput(speedCommand);
#else
  logOutputPreviewIfChanged(targetPercent_, status_);
#endif
}

DriverStatus CytronMd20aDriver::status() const { return status_; }

int CytronMd20aDriver::clampTargetPercent(int targetPercent) const {
  if (targetPercent < kTargetPercentMin) {
    return kTargetPercentMin;
  }
  if (targetPercent > kTargetPercentMax) {
    return kTargetPercentMax;
  }
  return targetPercent;
}

int CytronMd20aDriver::mapTargetPercentToSpeedCommand(int targetPercent) const {
  const int safeTargetPercent = clampTargetPercent(targetPercent);
  return (safeTargetPercent * kCytronSpeedMax) / kTargetPercentMax;
}

void CytronMd20aDriver::applyZeroOutput() {
#if ENABLE_REAL_MOTOR_OUTPUT
  if (motor_ != nullptr) {
    motor_->setSpeed(0);
  }
#endif
}

void CytronMd20aDriver::writeSpeedOutput(int speedCommand) {
#if ENABLE_REAL_MOTOR_OUTPUT
  if (speedCommand < kCytronSpeedMin || speedCommand > kCytronSpeedMax) {
    status_ = DriverStatus::Fault;
    targetPercent_ = 0; // Fault時はターゲットを即座にリセットする
    applyZeroOutput();
    Serial.println("MD20A Fault: speed command is out of range.");
    return;
  }

  // TODO: 安全設計 (VAMeter-Edu由来)
  // 急激な方向反転(正転<->逆転)による過電流を防ぐため、
  // 直前の出力符号と現在の出力符号が異なる場合は、
  // 一度 setSpeed(0) を出力し、数msのdelayまたは次周期まで待つ処理を検討する。
  // 現在は即時切り替わるため、負荷の大きいモータでは電源リセットを誘発する恐れがある。
  motor()->setSpeed(speedCommand);
#else
  (void)speedCommand;
#endif
}

#if ENABLE_REAL_MOTOR_OUTPUT
CytronMD *CytronMd20aDriver::motor() {
  if (motor_ == nullptr) {
    // 実出力有効時だけ生成し、begin()では停止値だけを明示する。
    motor_ = new CytronMD(PWM_DIR, pwmPin_, dirPin_);
  }
  return motor_;
}
#endif

bool CytronMd20aDriver::pinsAreConfigured() const {
  return pwmPin_ >= 0 && dirPin_ >= 0;
}

void CytronMd20aDriver::logOutputPreviewIfChanged(int targetPercent,
                                                  DriverStatus driverStatus) {
  if (targetPercent == lastLoggedTargetPercent_ &&
      driverStatus == lastLoggedStatus_) {
    return;
  }

  lastLoggedTargetPercent_ = targetPercent;
  lastLoggedStatus_ = driverStatus;

  const int speedCommand = mapTargetPercentToSpeedCommand(targetPercent);

  Serial.printf("MD20A Cytron preview: status=%d target=%d speed=%d output=%s\n",
                static_cast<int>(driverStatus), targetPercent, speedCommand,
#if ENABLE_REAL_MOTOR_OUTPUT
                "real"
#else
                "log-only"
#endif
  );
}
