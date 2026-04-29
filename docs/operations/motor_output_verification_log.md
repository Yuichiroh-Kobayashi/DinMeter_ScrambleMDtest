# Motor Output Verification Log

実出力(`ENABLE_REAL_MOTOR_OUTPUT=1`)を有効化する際は、以下のGate AおよびGate Bの段階を踏んで検証を行い、結果を記録すること。

## Verification Gates

### Gate A: No-motor Waveform Verification (モータ未接続・波形確認)
**目的**: モータを接続せず、ドライバの出力波形(PWM/DIR等)が想定通りかオシロスコープ等で確認する。
**重要**: この段階では、ドライバの出力端子に**モータを絶対に接続しない**こと。

- **Date**: YYYY-MM-DD
- **Target Device**: (e.g., MAKER-DRIVE)
- **Checklist**:
  - [ ] `Disabled` 時に出力信号が完全にLow(または停止値)である。
  - [ ] `Armed` かつ `target=0` で出力が停止値である。
  - [ ] `Armed` で `target>0` / `target<0` のときに、正しいピンから適切なPWM波形が出ている。
  - [ ] 電圧・周波数がドライバの仕様に適合している。
- **Result/Notes**: (結果や気付きを記述)

---

### Gate B: Real Output Verification (実モータ接続確認)
**目的**: 実際にモータを接続し、動作を確認する。
**前提条件**: **Gate A (No-motor Waveform Verification) がパスしていること。**

#### 2. No-load Motor Verification (無負荷モータ確認)
- **Date**: YYYY-MM-DD
- **Checklist**:
  - [ ] 起動時(Power-on)にモータが回らないこと。
  - [ ] `Disabled` 状態へ遷移した瞬間にモータが停止すること。
  - [ ] `Fault` 発生時(通信切断や異常検知)にモータが停止すること。
  - [ ] 正転・逆転の指令が正しく反映されること。
- **Result/Notes**: (結果や気付きを記述)

#### 3. Loaded Motor Verification (負荷付き確認)
- **Date**: YYYY-MM-DD
- **Checklist**:
  - [ ] 負荷をかけた状態で、ドライバの定格電流(Continuous current)を超えないこと。
  - [ ] 連続稼働で異常な発熱がないこと。
  - [ ] 急制動時に電源への逆起電力の影響が許容範囲内であること。
- **Result/Notes**: (結果や気付きを記述)

---

## Authorization for `ENABLE_REAL_MOTOR_OUTPUT=1`

通常のプロジェクト作業において、デフォルトビルドで `ENABLE_REAL_MOTOR_OUTPUT=1` を使用することは禁止されている。
実機bring-upを行う作業者のみが、上記の検証ログ(最低でもGate Aを完了させ、Gate Bへ進む)を記載した上で、ローカルで有効化してテストを行うこと。
