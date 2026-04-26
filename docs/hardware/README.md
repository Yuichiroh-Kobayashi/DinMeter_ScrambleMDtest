# Hardware Documentation

このディレクトリには、各デバイスの仕様書、接続図、通信プロトコルなどの情報を整理します。

## モータ・ドライバ一覧

| デバイス名 | 通信/制御方式 | フォルダ/ドキュメント | 備考 |
| :--- | :--- | :--- | :--- |
| RobStride EDULITE 05 | CAN | `RobStride_EDULITE_05/` | |
| DJI M3508 + C620 | CAN | `M3508_C620/` | |
| DDT-M0602C234 | RS485 | `DDT-M0602C234/` | |
| Cytron MD20A | PWM + DIR | `MD20A/` | |
| T-MOTOR AIR 40A | PWM | `TMOTOR_AIR_40A/` | |

## ドキュメントの配置ルール

1.  各フォルダに PDF 仕様書を配置してください。
2.  **重要**: AIが内容を把握できるよう、各フォルダに `protocol.md` や `spec.md` という名前で、以下の情報をテキストで書き出していただけると非常に助かります。
    *   ピンアサイン
    *   通信速度（Baudrate / CAN bit-rate）
    *   使用するレジスタやコマンド ID
    *   スケーリング（例：1000 = 10.0A など）
