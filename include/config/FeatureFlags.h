/**
 * @file
 * @brief 実験用機能のcompile-time flag定義。
 */

#ifndef FEATURE_FLAGS_H
#define FEATURE_FLAGS_H

/**
 * @brief 実モータ出力を有効化する明示flag。
 *
 * 既定値は0。1にしない限りMD20AやMAKER-DRIVE等のCytron系モータドライバのGPIO/LEDC出力処理はコンパイル対象外になり、
 * Serialログだけで動作する。
 */
#ifndef ENABLE_REAL_MOTOR_OUTPUT
#define ENABLE_REAL_MOTOR_OUTPUT 0
#endif

/**
 * @brief 起動時に使用するモータプロファイルを選択するフラグ。
 *
 * 0: MD20A (既定)
 * 1: MAKER-DRIVE Single Motor Mode
 */
#ifndef DEFAULT_MOTOR_PROFILE_MAKER_DRIVE
#define DEFAULT_MOTOR_PROFILE_MAKER_DRIVE 0
#endif

#endif // FEATURE_FLAGS_H
