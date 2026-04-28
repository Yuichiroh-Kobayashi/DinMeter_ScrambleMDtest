/**
 * @file
 * @brief モータドライバの状態を表す共通定義。
 */

#ifndef DRIVER_STATUS_H
#define DRIVER_STATUS_H

/**
 * @brief ドライバ内部の安全状態。
 */
enum class DriverStatus {
  Disabled, ///< 出力停止状態。
  Armed,    ///< update()で出力指令を扱える状態。
  Fault     ///< 異常状態。出力はゼロまたは未接続相当にする。
};

#endif // DRIVER_STATUS_H
