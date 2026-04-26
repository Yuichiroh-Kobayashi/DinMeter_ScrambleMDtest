#ifndef APP_STATE_H
#define APP_STATE_H

/**
 * @brief モータコントローラの状態管理用列挙型
 */
enum class SystemState {
    Disabled, ///< 出力停止状態（初期状態）
    Armed,    ///< 出力許可状態
    Fault     ///< 異常検知状態
};

#endif // APP_STATE_H
