# Motor Output Backend Design

この文書では、実モータ制御へ向けた段階的なバックエンド設計を定義する。
開発・検証のフェーズに応じて、安全にバックエンドを移行していくこと。

## Backend Stages

### 1. No-output Scaffold
- 構造やクラス定義だけが存在し、内部に制御ロジックを持たない。
- GPIO初期化や状態遷移の骨組みのみ。

### 2. No-op Backend
- 制御関数(例: `setSpeed`, `update`)は存在するが、物理的なI/O操作を一切行わない。
- 状態遷移やUIとの結合テストに使用する。

### 3. Preview / Log-only Backend
- `ENABLE_REAL_MOTOR_OUTPUT=0` の状態。
- 物理出力の代わりに、現在の状態・ターゲット値・出力予定のコマンドをSerialログへ出力する。
- 期待通りの制御値が計算されているか確認する。
- 現在のDinMeter_ScrambleMDtestのデフォルト。

### 4. No-motor Waveform Backend
- `ENABLE_REAL_MOTOR_OUTPUT=1` を有効化するが、**モータを物理的に接続しない**状態。
- オシロスコープ等を用いて、PWM信号や方向指定信号が正しく出力されているか、信号ピンだけで波形を確認する。
- ソフトウェアが「出力しているつもり」の状態を、安全に電気信号として検証する段階。

### 5. Real Output Backend
- `ENABLE_REAL_MOTOR_OUTPUT=1` かつ、**実際にモータを接続した**状態。
- 最初は無負荷(no-load)で確認し、その後に負荷(load)をかけて確認する。
- この段階へ進むには、フェーズ4での波形検証が完了していることが必須。

## Progression to Real Output

`ENABLE_REAL_MOTOR_OUTPUT=1` を有効にし、**Real Output Backend (フェーズ5)** へ進むには、以下の条件をすべて満たすこと。

1. **Preview / Log-only** において、意図しないタイミング(Disabled/Fault等)で非ゼロの速度コマンドが生成されないことがログで確認されている。
2. **No-motor Waveform** の確認が完了しており、PWMや制御信号の電圧・周波数・デューティ比が正常である。
3. 安全ログ(`docs/operations/motor_output_verification_log.md`)にフェーズ4までの確認結果が記録されている。
4. 電源やモータが安全な規格内(過電流・過電圧にならないこと)である。
5. 予期せぬ回転時に即座に電源を遮断できる手段(キルスイッチ等)が用意されている。
