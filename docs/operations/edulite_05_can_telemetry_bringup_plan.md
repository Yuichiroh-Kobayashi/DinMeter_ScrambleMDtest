# EDULITE 05 CAN Telemetry Bring-up Plan

## 目的

RobStride EDULITE 05（本線候補として評価中）の CAN 通信確認および Telemetry 取得確認を行う計画書である。

> [!IMPORTANT]
> - EDULITE 05 は本線候補として評価する。採用済みとは扱わない。
> - 本計画は Phase 1（通信確認）および Phase 2（Telemetry取得確認）に限定する。
> - **Enableコマンド（Type 3: Motor Enable）は送信しない。**
> - **spd_ref、トルク指令、電流指令は送信しない。**
> - 速度モードbring-up成功をもって定力制御可能とはみなさない。
> - 20V運用では最高速不足の実測があるが、初期Telemetry確認では速度性能を評価しない。

---

## 試験前提

本試験の前に、`docs/operations/dd_motor_preflight_checklist.md` を完了すること。

---

## 通信仕様

| 項目 | 値 |
|---|---|
| バス | CAN 2.0 |
| Bitrate | 1 Mbps |
| Frame format | Extended frame (29-bit ID) |
| Protocol | Private protocol |
| Data length | 8 bytes |

### 29-bit ID 構造

```
Bit[28:24] = communication type
Bit[23:8]  = data area 2
Bit[7:0]   = destination address (motor_id)
```

---

## 物理接続

### CAN transceiver

| 項目 | 値 |
|---|---|
| ESP32-S3 側 | TWAI (内蔵CANコントローラ) |
| Transceiver | 外付けCAN transceiver（型番: 未確定） |
| CAN_TX GPIO | G13 (PORT.A Yellow) — Candidate |
| CAN_RX GPIO | G15 (PORT.A White) — Candidate |

> [!NOTE]
> GPIO割当は `io_profile_matrix.md` に基づく Candidate 状態である。実機確認前に Provisional へ昇格させること。

### モータ側接続

| 端子 | 接続先 | 備考 |
|---|---|---|
| VBAT+ | 外部電源 + | DinMeterから供給しない |
| GND | 外部電源 - / Signal GND共通 | |
| CAN_H | CAN transceiver CAN_H | |
| CAN_L | CAN transceiver CAN_L | |

### 電源

| 項目 | 値 |
|---|---|
| 動作電圧範囲 | 15〜60 VDC |
| 定格電圧 | 48 VDC |
| 試験時電圧 | 実機条件を記録する（未確定） |
| 電流制限 | 電源側で設定。値を記録する |
| 電源遮断手段 | スイッチまたはコネクタ抜き |

> [!WARNING]
> 使用電源電圧は実機条件に基づき記録すること。DinMeterのPort A/B (5V) からモータ電源を供給しないこと。

### 終端抵抗

| 項目 | 値 |
|---|---|
| DinMeter側（transceiver端） | 120Ω |
| モータ側 | 120Ω（モータ内蔵の有無は未確認） |
| 確認 | オシロスコープまたはテスタで信号品質を確認する |

> [!NOTE]
> EDULITE 05 内部に終端抵抗が内蔵されているかは未確認。マニュアルで確認するか、実機測定で判断すること。

---

## ID設定

| 項目 | 値 |
|---|---|
| motor_id | 実機設定値。未確認なら 1 から確認する |
| host_id / master_id | 0xFD など固定値。実装時に統一する |

> [!NOTE]
> motor_id の実機初期値は未確認。試験開始前にID読み出しまたは接続確認を行うこと。

---

## Phase 1: 通信確認

### 目的

DinMeter（ESP32-S3 TWAI）からEDULITE 05への CAN フレーム送受信が成立するかを確認する。

### 確認項目

| No | 項目 | 方法 | 期待結果 | 結果 |
|---:|---|---|---|---|
| 1 | TWAI初期化 | ESP32-S3 TWAI ドライバを 1 Mbps / Extended frame で初期化 | エラーなく初期化完了 | |
| 2 | CAN bus idle | オシロスコープで CAN_H / CAN_L のバスアイドル状態を確認 | CAN_H ≈ 2.5V, CAN_L ≈ 2.5V (recessive) | |
| 3 | ID読み出し | Type 17 (param read) または Type 0 (info) フレームを送信 | EDULITE 05 から応答フレームを受信 | |
| 4 | 応答解析 | 受信フレームの 29-bit ID と 8-byte data を Serial に出力 | motor_id、communication type が期待通り | |

### 禁止事項

- Type 3 (Motor Enable) を送信しない
- Type 18 の spd_ref (0x700A) への書き込みを行わない
- いかなる非ゼロ指令も送信しない

---

## Phase 2: Telemetry取得確認

### 目的

EDULITE 05 が自発的に送信する、または読み出し要求に応じて返すTelemetry値を取得・解釈できるかを確認する。

### 取得したいTelemetry候補

| Index | Name | Type | 内容 | Semantics | 優先度 |
|---:|---|---|---|---|---|
| 0x701C | VBUS | float | バス電圧 | BusVoltage_V | 高 |
| 0x701B | mechVel | float | 負荷側速度 | Feedback_rad_s | 中 |
| — | fault_code | — | Fault / Warning ビットフィールド | FaultCode | 高 |
| — | motor_temp | — | モータ温度（取得可否は未確認） | MotorTemp_C / unknown | 中 |
| — | run_mode | uint8 | 現在の制御モード | RunMode | 中 |

> [!NOTE]
> Telemetry の取得方法（自発送信 / 読み出し要求）は Private protocol のフレームタイプに依存する。Type 2 (feedback) や Type 17 (param read) を使用する想定だが、実機で確認するまで未検証。

### 確認項目

| No | 項目 | 方法 | 期待結果 | 結果 |
|---:|---|---|---|---|
| 1 | VBUS読み出し | Type 17 で Index 0x701C を読み出し | バス電圧が実測電源電圧と一致（概算） | |
| 2 | Fault状態確認 | 応答フレームの fault ビットフィールドを解析 | 正常時はFaultなし（bit = 0） | |
| 3 | mechVel読み出し | Type 17 で Index 0x701B を読み出し | 停止中なら ≈ 0 rad/s | |
| 4 | 手動回転時のmechVel | 治具や安全なつまみ等で軸を低速に回し、mechVelが変化するか確認 | 回転方向に応じて正負の速度値 | |

> [!CAUTION]
> Phase 2 の手動回転確認では、**手や指を軸に巻き込まれる位置に置かない**こと。治具や安全なつまみを使用する。

---

## Go / NoGO 条件

### GO（Phase 1/2 成功・継続条件）

以下のすべてを満たす場合、Phase 3（最小指令送信）の計画策定に進んでよい。

- CAN 1 Mbps / Extended frame で DinMeter ↔ EDULITE 05 間の通信が確立できた
- motor_id を特定し、応答フレームの形式が Private protocol の仕様と一致した
- VBUS の読み出し値が実測電源電圧と概ね一致した
- Fault ビットフィールドが正常（異常なし）であった
- mechVel が停止中 ≈ 0、手動回転時に変化することを確認できた
- 通信中に bus-off や頻繁なエラーフレームが発生しなかった

### NoGO（不採用・見直し条件）

以下のいずれかに該当する場合、Phase 3 には進まない。

- CAN通信が確立できない（TWAI初期化エラー、バス信号なし、応答なし）
- motor_id が特定できない
- 応答フレームの形式が仕様と一致しない
- VBUS 読み出し値が実測電圧と大幅に乖離する
- 原因不明の bus-off やエラーフレームが頻発する
- Fault ビットが常時立っており、原因を特定できない

---

## 撤退条件

以下に該当する場合は試験を中止し、原因調査に移る。

- CAN transceiver またはモータからの異常発熱・異臭
- DinMeter のリセットやフリーズ
- 電源電流が想定を大幅に超過（Enableしていないにもかかわらず）
- バス信号がまったく観測できない

---

## 次に進む条件

Phase 1/2 が GO であった場合、Phase 3（最小指令送信）に進むためには以下が必要である。

1. **レビュー**: Phase 1/2 の試験結果ログを作成し、レビューを受けること
2. **Enableコマンド送信計画**: Phase 3 用の安全計画を別途作成すること（本文書の範囲外）
3. **電流制限設定**: `limit_cur` の初期値を安全な値（例: 1.0 A）に設定する手順を計画すること
4. **Safety State連携**: DinMeter 側の `Disabled → Armed` 遷移と EDULITE 05 の Enable/Disable が連動する設計を確認すること
5. **Fault遷移**: CAN timeout / bus-off / fault frame / feedback途絶で `Fault` へ遷移する設計を確認すること

> [!IMPORTANT]
> Enable送信に進む前に、試験結果ログのレビューと、Phase 3用の安全計画の承認を得ること。

---

## 未確定事項

| 項目 | 状態 |
|---|---|
| CAN transceiver 型番 | 未確定 |
| GPIO割当 (CAN_TX / CAN_RX) | Candidate（io_profile_matrix.md） |
| motor_id の初期値 | 未確認 |
| host_id の値 | 未確定（0xFD 想定） |
| モータ内蔵終端抵抗の有無 | 未確認 |
| Private protocol の全communication type一覧 | マニュアル参照が必要 |
| 試験時電源電圧 | 未確定（15〜60VDC範囲内） |
| 通信タイムアウト実装値 | 未確定 |
| Telemetry自発送信の有無とタイミング | 未検証 |
