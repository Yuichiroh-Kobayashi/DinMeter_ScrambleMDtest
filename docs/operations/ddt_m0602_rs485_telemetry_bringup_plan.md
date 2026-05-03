# DDT-M0602C234 RS485 Telemetry Bring-up Plan

## 目的

DDT-M0602C234（本線候補として評価中）の RS485 通信確認および Telemetry 取得確認を行う計画書である。

> [!IMPORTANT]
> - DDT-M0602C234 は本線候補として評価する。採用済みとは扱わない。
> - 本計画は Phase 1（通信確認）および Phase 2（Telemetry取得確認）に限定する。
> - **Enableコマンド（駆動指令 Protocol 1 の非ゼロ速度指令）は送信しない。**
> - **速度指令、トルク指令、電流指令は送信しない。**
> - 速度環モードの動作確認成功をもって定力制御可能とはみなさない。
> - current / torque / temperature / speed の semantics を混同しない。

---

## 試験前提

本試験の前に、`docs/operations/dd_motor_preflight_checklist.md` を完了すること。

---

## 通信仕様

| 項目 | 値 |
|---|---|
| バス | RS485 半二重 |
| Baudrate | 57600 bps |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | None |
| Data length | 10 bytes |
| 通信方式 | 一問一答（Master送信 → Slave応答） |
| 最大更新レート | 500 Hz |
| CRC | CRC-8/MAXIM over DATA[0]〜DATA[8] |

---

## 物理接続

### RS485 transceiver

| 項目 | 値 |
|---|---|
| ESP32-S3 側 | UART |
| Transceiver | 外付け RS485 transceiver（型番: 未確定） |
| UART TX GPIO | G2 (PORT.B Yellow) — Candidate |
| UART RX GPIO | G1 (PORT.B White) — Candidate |
| DE/RE 制御ピン | 未確定 |

> [!NOTE]
> - GPIO割当は `io_profile_matrix.md` に基づく Candidate 状態である。実機確認前に Provisional へ昇格させること。
> - MAKER-DRIVE (PWM) と DDT RS485 は Port B を共有するため、同時使用しない。プロファイル切替で排他制御する。
> - RS485半二重通信には DE/RE 制御が必要。制御ピンは未確定。実装時に `io_profile_matrix.md` と整合させること。

### モータ側接続

| 線番号 | 名称 | 接続先 | 備考 |
|---:|---|---|---|
| 1 | A / DATA+ | RS485 transceiver A | |
| 2 | B / DATA- | RS485 transceiver B | |
| 3 | VCC | 外部電源 + | DinMeterから供給しない |
| 4 | GND | 外部電源 - / Signal GND | |

### 電源

| 項目 | 値 |
|---|---|
| 動作電圧範囲 | 20〜30 VDC |
| 定格電圧 | 25.6 VDC |
| 試験時電圧 | 実機条件を記録する（未確定） |
| 電流制限 | 電源側で設定。値を記録する |
| 電源遮断手段 | スイッチまたはコネクタ抜き |

> [!WARNING]
> モータ電源(20〜30V)をDinMeterのPort A/B (5V) から供給しないこと。

### GND方針

| 項目 | 確認内容 |
|---|---|
| Signal GND共通化 | DinMeter GND とモータ電源 GND を共通にする |
| 接続点 | RS485 transceiver の GND をモータ GND と共通にする |

### 終端抵抗

| 項目 | 値 |
|---|---|
| RS485 終端抵抗 | 要否をデータシートで確認する（未確認） |
| 備考 | 短距離・2ノード構成では不要な場合があるが、信号品質に問題がある場合は A-B 間に 120Ω を追加する |

---

## ID設定

| 項目 | 値 |
|---|---|
| モータID | 実機設定値。未確認なら ID確認手順で特定する |
| 備考 | spec.md に「左右モータID 04/05」の記載あり。単体試験用IDへの変更可否は未確定 |

> [!NOTE]
> モータIDの実機初期値は未確認。接続前にID読み出しまたはID設定手順をマニュアルで確認すること。

---

## CRC-8/MAXIM

| 項目 | 値 |
|---|---|
| 多項式 | 0x31 (x^8 + x^5 + x^4 + 1) |
| 初期値 | 0x00 |
| 入力反転 | Yes |
| 出力反転 | Yes |
| 対象バイト | DATA[0]〜DATA[8]（9バイト） |
| 格納位置 | DATA[9] |

> [!NOTE]
> CRC計算の正確な実装は、マニュアル記載のサンプルまたは既知のCRC-8/MAXIMテーブルで検証すること。

---

## Phase 1: 通信確認

### 目的

DinMeter（ESP32-S3 UART + RS485 transceiver）から DDT-M0602C234 への フレーム送受信が成立するかを確認する。

### 確認項目

| No | 項目 | 方法 | 期待結果 | 結果 |
|---:|---|---|---|---|
| 1 | UART初期化 | ESP32-S3 UART を 57600/8N1 で初期化 | エラーなく初期化完了 | |
| 2 | DE/RE制御確認 | RS485 transceiver の DE/RE 制御が送信/受信切替に対応すること | 送信時 DE=H、受信時 RE=L | |
| 3 | Protocol 2 送信 | 安全なフィードバック取得要求 (0x74) を送信 | 10バイトの応答フレームを受信 | |
| 4 | CRC検証 | 受信フレームの DATA[9] を CRC-8/MAXIM で検証 | CRCが一致 | |
| 5 | ID確認 | 応答フレームの DATA[0] がモータIDと一致 | ID一致 | |

### Protocol 2: フィードバック取得要求

送信フレーム（10バイト）:

| Byte | 値 | 内容 |
|---:|---|---|
| 0 | motor_id | モータID |
| 1 | 0x74 | Protocol 2 コマンド |
| 2〜8 | 0x00 | 未使用 |
| 9 | CRC | CRC-8/MAXIM |

> [!NOTE]
> Protocol 2 (0x74) は駆動指令を含まない安全なフィードバック取得要求である。Protocol 1 (0x64) は駆動指令を含むため、Phase 1/2 では使用しない。

### 禁止事項

- Protocol 1 (0x64) に非ゼロの速度/電流/位置指令を含めて送信しない
- ブレーキ解除指令を送信しない

---

## Phase 2: Telemetry取得確認

### 目的

DDT-M0602C234 から返されるフィードバックフレームを正しく解釈できるかを確認する。

### 取得したいTelemetry候補

Protocol 1 応答（駆動指令非送信でも一部取得できる可能性あり。実機で確認が必要）:

| Byte | 名称 | Semantics | 備考 |
|---:|---|---|---|
| 0 | ID | MotorID | |
| 1 | mode | RunMode | 現在の制御モード |
| 2-3 | torque_current | phase_current (未検証) | 「トルク電流」の意味はデータシートに依存。相電流なのかq軸電流なのかは未検証 |
| 4-5 | speed | Feedback_rpm | 速度フィードバック |
| 6-7 | position | Position_deg | 位置フィードバック (0〜360°) |
| 8 | fault_code | FaultCode | ビットフィールド |
| 9 | CRC | CRC8 | |

Protocol 2 応答:

| 内容 | Semantics | 備考 |
|---|---|---|
| winding temperature | WindingTemp_C | |
| U8 position | Position_U8 | |

> [!CAUTION]
> - `torque_current` の名称はデータシート由来だが、これが「出力トルク」を直接表すとは限らない。semanticsは「phase_current（未検証）」として扱う。
> - current / torque / temperature / speed の値は、それぞれのsemanticsを混同しないこと。

### 確認項目

| No | 項目 | 方法 | 期待結果 | 結果 |
|---:|---|---|---|---|
| 1 | Protocol 2 応答取得 | 0x74 を送信し、応答を受信 | 10バイトの応答を受信、CRC一致 | |
| 2 | ID一致 | 応答の Byte[0] がモータIDと一致 | 一致 | |
| 3 | mode確認 | 応答の Byte[1] がモード値（0x01/0x02/0x03等） | 妥当なモード値 | |
| 4 | fault_code確認 | 応答の Byte[8] が 0x00（正常） | 異常なし | |
| 5 | winding temperature | Protocol 2 応答から温度を取得 | 室温付近の妥当な値 | |
| 6 | 手動回転時の速度変化 | 治具等で軸を低速に回し、speed値の変化を確認 | 回転方向に応じて速度値が変化 | |

### Timeout条件

| 項目 | 値 |
|---|---|
| 応答タイムアウト | 送信後 50ms 以内に 10バイト受信できなければタイムアウト |
| リトライ | 3回まで |
| タイムアウト連続発生 | Fault扱い |

> [!CAUTION]
> Phase 2 の手動回転確認では、**手や指を軸に巻き込まれる位置に置かない**こと。治具や安全なつまみを使用する。

---

## Go / NoGO 条件

### GO（Phase 1/2 成功・継続条件）

以下のすべてを満たす場合、Phase 3（最小指令送信）の計画策定に進んでよい。

- RS485 57600bps でフレーム送受信が成立した
- CRC-8/MAXIM が一致した
- モータIDを特定できた
- Protocol 2 でフィードバック応答を取得できた
- fault_code が正常であった
- 手動回転時に速度値が変化した
- タイムアウトやCRCエラーが頻発しなかった
- 温度値が室温付近の妥当な値であった

### NoGO（不採用・見直し条件）

以下のいずれかに該当する場合、Phase 3 には進まない。

- RS485通信が確立できない（UART初期化エラー、応答なし）
- CRC不一致が常時発生する
- モータIDが特定できない
- Protocol 2 応答が仕様と一致しない
- fault_code が常時異常で原因特定できない
- 通信タイムアウトが頻発する

---

## 撤退条件

以下に該当する場合は試験を中止し、原因調査に移る。

- RS485 transceiver またはモータからの異常発熱・異臭
- DinMeter のリセットやフリーズ
- 電源電流が想定を大幅に超過
- バス信号がまったく観測できない
- モータが予期しない動作をした場合（Enableしていないにもかかわらず）

---

## 次に進む条件

Phase 1/2 が GO であった場合、Phase 3（最小指令送信）に進むためには以下が必要である。

1. **レビュー**: Phase 1/2 の試験結果ログを作成し、レビューを受けること
2. **Protocol 1 送信計画**: Phase 3 用の安全計画を別途作成すること（本文書の範囲外）
3. **速度指令の安全限界**: speed loop の初期指令値を安全な値（最小 rpm）に設定する計画を立てること
4. **Safety State連携**: DinMeter側の `Disabled → Armed` 遷移と DDT-M0602C234 の制御開始/停止が連動する設計を確認すること
5. **Fault遷移**: RS485 timeout / CRC error / fault_code 異常 / 応答途絶で `Fault` へ遷移する設計を確認すること
6. **Semantics確認**: `torque_current` が何を意味するか（相電流 / q軸電流 / 出力トルク相当か）の判断材料を整理すること

> [!IMPORTANT]
> Enable送信（速度指令送信）に進む前に、試験結果ログのレビューと、Phase 3用の安全計画の承認を得ること。

---

## 未確定事項

| 項目 | 状態 |
|---|---|
| RS485 transceiver 型番 | 未確定 |
| DE/RE 制御ピン | 未確定（io_profile_matrix.md に記載なし） |
| GPIO割当 (UART TX / RX) | Candidate（io_profile_matrix.md） |
| モータIDの初期値 | 未確認 |
| ID 04/05 を単体試験用に変更するか | 未確定 |
| 試験時電源電圧 | 未確定（20〜30VDC範囲内） |
| 通信タイムアウト実装値 | 50ms 想定（要実機検証） |
| `torque_current` の正確な semantics | 未検証（相電流 / q軸電流 / 他） |
| Protocol 1 応答が駆動指令なしで取得できるか | 未検証 |
| RS485終端抵抗の要否 | 未確認 |
