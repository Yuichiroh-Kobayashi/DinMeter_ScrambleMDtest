# DD Motor Constant Force Handoff

## 目的

本ドキュメントは、VAMeter-Edu / Motor Observe 開発で得られた知見を基に、DinMeter_ScrambleMDtest における M5DinMeter とモータドライバ内蔵DD（ダイレクトドライブ）モータを組み合わせた「教育用可変型定力装置」の設計方針を整理したものである。

## 以前の構成からの移行理由

以前の検証（VAMeter-Edu + MAKER-DRIVE + 減速機付きDCモータ）から得られた知見により、以下の構成および考え方は**定力装置の本線候補から除外（撤退）**された。

- 減速機付きDCモータの利用（静止摩擦、歯車抵抗、非線形性による影響）
- PWM値を力（force）とみなすこと
- driver input current（ドライバ入力電流）を出力トルクとみなすこと
- VAMeterをHブリッジ出力のモータ端子間に直接接続し、motor terminal measurement（モータ端子測定）を行ったと記述すること

## 本線候補ハードウェア

DinMeter_ScrambleMDtest では、定力装置の本線候補として以下の「モータドライバ内蔵DDモータ」を採用する。

1. **RobStride EDULITE 05** (CAN通信)
2. **DDT-M0602C234** (RS485通信)

これらのモータは、通信経由で制御コマンド（速度・トルク指令など）を送信でき、同時にモータドライバ内部で測定された電流・速度・位置などのTelemetry（テレメトリ）を取得できる利点がある。

## 引き継ぐ設計資産

以前の構成からは撤退するが、以下のソフトウェア・運用設計資産は本プロジェクトへ引き継ぐ。

- **Safety State Machine** (`Disabled` -> `Armed` の状態遷移と安全停止条件)
- **Requested Command と Applied Command の分離**
- **Telemetryと意味付け（Semantics）を伴うCSV記録**
- **QRコードまたはローカル通信によるデータ転送方式**
- **Go / NoGO / Rollback の判断基準設計**
- **「unknown（不明）な値」を教材判断・授業の考察材料に使わないという原則**

## 基本方針

- ハードウェア固有のTelemetry（ドライバが報告する相電流やトルク推定値）を活用する。
- 物理量（力、トルク、電流）を記録する際は、必ずその「意味（Semantics）」を併記する。
- 低トルク・低速域での安全な立ち上げ（Bring-up）を最優先する。
