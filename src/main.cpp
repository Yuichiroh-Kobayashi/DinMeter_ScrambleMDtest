/**
 * @file main.cpp
 * @brief M5Stack DinMeter v1.1 簡易モータコントローラ Bring-up用コード
 *
 * 安全仕様:
 * - 電源投入時は必ず Disabled。
 * - target = 0 の時のみ長押しで Armed 可能。
 * - 今回は実際のモータ出力（CAN/RS485/PWM）は行わない。
 */

#include "AppState.h"
#include <M5DinMeter.h>

// 状態管理
SystemState currentState = SystemState::Disabled;
int32_t targetValue = 0;
const int32_t TARGET_MIN = -100;
const int32_t TARGET_MAX = 100;

// 前回の値を保持（表示更新用）
SystemState lastState = SystemState::Fault; // 初期表示を強制するため
int32_t lastTarget = -999;

/**
 * @brief 画面表示の更新
 */
void updateDisplay() {
  if (currentState == lastState && targetValue == lastTarget)
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
  DinMeter.Display.printf("Target: %4d", targetValue);

  DinMeter.Display.endWrite();

  lastState = currentState;
  lastTarget = targetValue;
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
    targetValue += (diff);
    if (targetValue < TARGET_MIN)
      targetValue = TARGET_MIN;
    if (targetValue > TARGET_MAX)
      targetValue = TARGET_MAX;
    Serial.printf("Target updated: %d\n", targetValue);
  }

  // ボタン操作: 長押しで状態遷移
  if (DinMeter.BtnA.wasHold()) {
    if (currentState == SystemState::Disabled) {
      if (targetValue == 0) {
        currentState = SystemState::Armed;
        Serial.println("State Changed: ARMED");
      } else {
        Serial.println("Safety: Cannot Arm unless target is 0");
      }
    } else if (currentState == SystemState::Armed) {
      currentState = SystemState::Disabled;
      Serial.println("State Changed: DISABLED");
    }
  }

  // 短押しでDisabledに戻る（非常停止的な扱い）
  if (DinMeter.BtnA.wasClicked() && currentState == SystemState::Armed) {
    currentState = SystemState::Disabled;
    Serial.println("State Changed: DISABLED (by click)");
  }

  // 画面更新
  updateDisplay();

  delay(10);
}
