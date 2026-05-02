/**
 * @file main.cpp
 * @brief M5Stack DinMeter v1.1 簡易モータコントローラ Bring-up用コード
 *
 * 安全仕様:
 * - 電源投入時は必ず Disabled。
 * - target = 0 の時のみ長押しで Armed 可能。
 * - MD20AやMAKER-DRIVEの実出力はENABLE_REAL_MOTOR_OUTPUTが1のときだけ有効。
 */

#include "AppState.h"
#include "config/FeatureFlags.h"
#include "config/PinConfig.h"
#include "drivers/CytronMakerDriveDriver.h"
#include "drivers/CytronMd20aDriver.h"
#include <M5DinMeter.h>

enum class MotorProfile {
  MD20A,
  MAKER_DRIVE_SINGLE
};

#if DEFAULT_MOTOR_PROFILE_MAKER_DRIVE
constexpr MotorProfile kDefaultMotorProfile = MotorProfile::MAKER_DRIVE_SINGLE;
#else
constexpr MotorProfile kDefaultMotorProfile = MotorProfile::MD20A;
#endif

// 状態管理
SystemState currentState = SystemState::Disabled;
int32_t targetPercent = 0;
const int32_t TARGET_PERCENT_MIN = -100;
const int32_t TARGET_PERCENT_MAX = 100;

MotorDriver* activeDriver = nullptr;
CytronMd20aDriver md20aDriver(PinConfig::kMd20aPwmPin,
                              PinConfig::kMd20aDirPin);
CytronMakerDriveDriver makerDriveDriver(PinConfig::kMakerDriveM1aPin,
                                        PinConfig::kMakerDriveM1bPin,
                                        PinConfig::kMakerDriveM2aPin,
                                        PinConfig::kMakerDriveM2bPin,
                                        false); // false = single motor mode

// 前回の値を保持（表示更新用）
SystemState lastState = SystemState::Fault; // 初期表示を強制するため
int32_t lastTargetPercent = -999;

/**
 * @brief 画面表示の更新
 */
void updateDisplay() {
  if (currentState == lastState && targetPercent == lastTargetPercent)
    return;

  DinMeter.Display.startWrite();

  // 状態が変化した場合は背景含めてリセット
  if (currentState != lastState) {
    DinMeter.Display.fillScreen(BLACK);
    DinMeter.Display.setCursor(0, 10);
    DinMeter.Display.setTextSize(1.5);
    DinMeter.Display.setTextColor(WHITE);
    DinMeter.Display.printf(" DinMeter Motor Console\n");
    DinMeter.Display.drawFastHLine(0, 30, DinMeter.Display.width(), WHITE);
  }

  // 状態表示
  DinMeter.Display.setCursor(10, 50);
  DinMeter.Display.setTextSize(2);
  if (currentState == SystemState::Disabled) {
    DinMeter.Display.setTextColor(0x7BEF); // GRAY
    DinMeter.Display.print("State: Disabled");
  } else if (currentState == SystemState::Armed) {
    DinMeter.Display.setTextColor(RED);
    DinMeter.Display.print("State: ARMED   ");
  } else {
    DinMeter.Display.setTextColor(ORANGE);
    DinMeter.Display.print("State: FAULT   ");
  }

  // Target表示
  DinMeter.Display.setCursor(10, 90);
  DinMeter.Display.setTextSize(3);
  DinMeter.Display.setTextColor(CYAN, BLACK);
  DinMeter.Display.printf("Target: %4d", targetPercent);

  DinMeter.Display.endWrite();

  lastState = currentState;
  lastTargetPercent = targetPercent;
}

void setup() {
  // 1. 電源保持設定 (DinMeter v1.1 GPIO46)
  pinMode(46, OUTPUT);
  digitalWrite(46, HIGH);

  // 2. M5Stack初期化
  auto cfg = M5.config();
  DinMeter.begin(cfg, true); // エンコーダを有効化

  // 3. シリアル通信開始
  Serial.begin(115200);
  delay(500);
  Serial.println("--- DinMeter Motor Console Start ---");
  Serial.println("Board: M5Stack DinMeter v1.1 (Stamp-S3)");
  Serial.println("Status: Power Hold (GPIO46) set to HIGH");

  if (kDefaultMotorProfile == MotorProfile::MAKER_DRIVE_SINGLE) {
    activeDriver = &makerDriveDriver;
    Serial.println("Selected Profile: MAKER_DRIVE_SINGLE");
  } else {
    activeDriver = &md20aDriver;
    Serial.println("Selected Profile: MD20A");
  }

  if (!activeDriver->begin()) {
    currentState = SystemState::Fault;
    targetPercent = 0;
    Serial.println("State Changed: FAULT (driver begin failed)");
  }

  // 4. 画面初期設定
  DinMeter.Display.setRotation(1);
  DinMeter.Display.fillScreen(BLACK);

  Serial.println("Setup Complete.");
}

void loop() {
  DinMeter.update();

  // エンコーダ操作: target値の増減
  int32_t diff = DinMeter.Encoder.readAndReset();
  if (diff != 0) {
    if (currentState == SystemState::Fault) {
      targetPercent = 0;
      activeDriver->setTargetPercent(0);
      Serial.println("Safety: Target change ignored while Fault.");
    } else {
      targetPercent += (diff);
      if (targetPercent < TARGET_PERCENT_MIN)
        targetPercent = TARGET_PERCENT_MIN;
      if (targetPercent > TARGET_PERCENT_MAX)
        targetPercent = TARGET_PERCENT_MAX;
      activeDriver->setTargetPercent(static_cast<int>(targetPercent));
      Serial.printf("Target updated: %d\n", targetPercent);
    }
  }

  // ボタン操作: 長押しで状態遷移
  if (DinMeter.BtnA.wasHold()) {
    if (currentState == SystemState::Disabled) {
      if (targetPercent == 0) {
        if (activeDriver->arm()) {
          currentState = SystemState::Armed;
          Serial.println("State Changed: ARMED");
        } else {
          currentState = SystemState::Fault;
          targetPercent = 0;
          Serial.println("State Changed: FAULT (driver arm failed)");
        }
      } else {
        Serial.println("Safety: Cannot Arm unless target is 0");
      }
    } else if (currentState == SystemState::Armed) {
      currentState = SystemState::Disabled;
      activeDriver->disarm();
      Serial.println("State Changed: DISABLED");
    }
  }

  // 短押しでDisabledに戻る（非常停止的な扱い）
  if (DinMeter.BtnA.wasClicked() && currentState == SystemState::Armed) {
    currentState = SystemState::Disabled;
    activeDriver->disarm();
    Serial.println("State Changed: DISABLED (by click)");
  }

  if (currentState == SystemState::Disabled || currentState == SystemState::Fault) {
    activeDriver->disarm();
  }
  activeDriver->update();

  // 画面更新
  updateDisplay();

  delay(10);
}
