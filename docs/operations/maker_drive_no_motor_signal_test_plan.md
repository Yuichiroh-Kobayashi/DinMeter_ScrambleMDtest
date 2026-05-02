# MAKER-DRIVE No-motor Signal Verification Test Plan

## 1. 試験目的

Log-only（ソフトウェアシミュレーション）で計算された指令が、実際の物理的なGPIO信号として想定通りに出力されるかを確認する。

- **確認対象**: DinMeter GPIOからMAKER-DRIVE入力端子（M1A/M1B）へ送られる制御信号。
- **注意**: モータ出力端子側の電圧を確認する試験ではない。また、VAMeter-Eduでの試験結果を流用せず、DinMeter実機での動作を新規に検証する。

## 2. 試験の段階

### Gate A-0: DinMeter単体・GPIOピン単体での波形確認
- MAKER-DRIVEを接続せず、DinMeterのPORT.B（G2, G1）ピンの信号を直接測定する。
- 外部モータ電源、MAKER-DRIVEともに不要。

### Gate A-1: MAKER-DRIVE入力端子へ接続した状態での入力信号確認
- DinMeterとMAKER-DRIVEの信号線を接続した状態で、MAKER-DRIVE入力端子における信号波形を確認する。
- **DCモータは絶対に接続しない。**
- **外部モータ電源について**: MAKER-DRIVEのロジック回路を動作させるためにVB+への給電が必要な場合がある。その場合は、仕様範囲内の最小電圧（乾電池等）を使用し、安全を確保した上で実施する。

## 3. 試験前条件

- **ソフトウェア設定**:
  - `include/config/FeatureFlags.h` にて以下を一時的に有効化（コミット禁止）。
    - `ENABLE_REAL_MOTOR_OUTPUT 1`
    - `DEFAULT_MOTOR_PROFILE_MAKER_DRIVE 1`
- **物理構成**:
  - DCモータ：未接続
  - 回転物・治具：なし
  - 測定器：オシロスコープまたはロジックアナライザ
- **安全確保**:
  - GND基準を明確にする（DinMeterのGNDと測定器のGNDを共通にする）。
  - 指導者またはメンターの立ち会い推奨。

## 4. 確認信号（MAKER-DRIVE Single Motor Mode）

| 信号 | GPIO | 役割 |
|---|---|---|
| **M1A** | GPIO 2 (PORT.B Yellow) | PWM input A for motor 1 |
| **M1B** | GPIO 1 (PORT.B White) | PWM input B for motor 1 |

### 期待される挙動 (PWM_PWM方式)

| 状態 | Target値 | M1A (GPIO 2) | M1B (GPIO 1) | 備考 |
|---|---|---|---|---|
| **Disabled** | 任意 | **Low** | **Low** | 停止値 |
| **Fault** | 任意 | **Low** | **Low** | 停止値 |
| **Armed** | 0 | **Low** | **Low** | 停止値 |
| **Armed** | > 0 (正転) | **PWM** | **Low** | |
| **Armed** | < 0 (逆転) | **Low** | **PWM** | |

※ 初期bring-upでは `High/High`、`PWM/High`、`High/PWM` などのパターンは使用しない。

## 5. 記録項目 (Test Log Template)

試験実施時は以下の項目を記録する。

- **Date**:
- **Branch / Commit**:
- **FeatureFlags**: `ENABLE_REAL_MOTOR_OUTPUT=1`, `DEFAULT_MOTOR_PROFILE_MAKER_DRIVE=1` であることの確認。
- **測定器**:
- **測定点**: (例: DinMeter PORT.B ピン直下 / MAKER-DRIVE ネジ端子部)
- **GND接続**:
- **外部電源 (VB+)**: 有無および電圧。

| No | State | Target | M1A (Freq/Duty/V) | M1B (Freq/Duty/V) | Result |
|---:|---|---:|---|---|---|
| 1 | Disabled | 0 | | | |
| 2 | Armed | 0 | | | |
| 3 | Armed | 50 | | | |
| 4 | Armed | 100 | | | |
| 5 | Armed | -50 | | | |
| 6 | Armed | -100 | | | |
| 7 | Disabled | 100 | | | |

## 6. Go / NoGO 条件

以下のいずれかに該当する場合は **NoGO**（試験中止・実機接続禁止）とする。

- **Disabled** または **Fault** 状態で、非ゼロの波形（PWMまたはHigh）が出力される。
- **Armed** かつ `target=0` で波形が停止値（Low/Low）にならない。
- 電圧レベルがMAKER-DRIVEの入力仕様（High: 1.7〜6V, Low: 0〜0.5V）に適合しない。
- 予期しないシステムのリセット、フリーズ、または基板の異常発熱が発生する。
- **Hold (保留)**: targetの符号に対してPWM出力されるピンが逆である場合。配線または論理の修正が必要。

## 7. 撤退条件

- GPIO波形が著しく不安定で、Duty比の制御が困難な場合。
- 停止操作（ボタン短押し等）に信号が即座に反応しない場合。
- `FeatureFlags.h` の戻し忘れにより、意図せず実出力設定が残るリスクが高いと判断される場合。
- 接続条件が不明確なまま、外部電源の投入に不安がある場合。

## 8. 作業後に行うこと

1. **原状復帰**: `FeatureFlags.h` を既定値（両方 `0`）に必ず戻し、ビルド・書き込みを確認する。
2. **ログ保存**: `docs/operations/maker_drive_no_motor_signal_test_YYYYMMDD.md` として結果を保存。
3. **レビュー**: 結果をレビューし、Gate B（実モータ接続・無負荷試験）へ進むか判断する。
