/**
 * @file
 * @brief DinMeterとモータドライバを接続するピン定義。
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

namespace PinConfig {

/**
 * @brief MD20A PWM入力ピン。
 *
 * このプロジェクトではDinMeter PORT.B G2をMD20A PWMとして仮割当する。
 * CytronMotorDriver経由の出力波形確認は未検証。
 */
constexpr int kMd20aPwmPin = 2;

/**
 * @brief MD20A DIR入力ピン。
 *
 * このプロジェクトではDinMeter PORT.B G1をMD20A DIRとして仮割当する。
 * CytronMotorDriver経由の方向確認は未検証。
 */
constexpr int kMd20aDirPin = 1;

/**
 * @brief MAKER-DRIVE M1A入力ピン。
 *
 * このプロジェクトではDinMeter PORT.B G2をMAKER-DRIVE M1Aとして仮割当する。
 * CytronMotorDriver経由の出力波形確認は未検証。
 */
constexpr int kMakerDriveM1aPin = 2;

/**
 * @brief MAKER-DRIVE M1B入力ピン。
 *
 * このプロジェクトではDinMeter PORT.B G1をMAKER-DRIVE M1Bとして仮割当する。
 * CytronMotorDriver経由の出力波形確認は未検証。
 */
constexpr int kMakerDriveM1bPin = 1;

/**
 * @brief MAKER-DRIVE M2A入力ピン。
 *
 * このプロジェクトではDinMeter PORT.A G13をMAKER-DRIVE M2Aとして仮割当する。
 * 2ch modeはUI未実装であり、実機波形確認も未検証。
 */
constexpr int kMakerDriveM2aPin = 13;

/**
 * @brief MAKER-DRIVE M2B入力ピン。
 *
 * このプロジェクトではDinMeter PORT.A G15をMAKER-DRIVE M2Bとして仮割当する。
 * 2ch modeはUI未実装であり、実機波形確認も未検証。
 */
constexpr int kMakerDriveM2bPin = 15;

} // namespace PinConfig

#endif // PIN_CONFIG_H
