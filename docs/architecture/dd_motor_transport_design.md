# DD Motor Transport Design

## 目的

DinMeter_ScrambleMDtest における CAN / RS485 の通信トランスポート設計方針をまとめる。本文書はコード実装前の設計メモであり、コード変更は行わない。

---

## 設計原則

### 1. CAN と RS485 を同じドライバクラスに混ぜない

CAN（EDULITE 05）と RS485（DDT-M0602C234）は、物理層・フレーム構造・エラー処理が根本的に異なる。これらを共通のドライバクラスに混在させると、安全設計の見通しが悪くなる。

```
NG: class UnifiedMotorDriver { /* CAN も RS485 も内部で分岐 */ }

OK:
  class CanEduLitePrivateDriver : public MotorDriver { ... }
  class Rs485DdtM0602Driver     : public MotorDriver { ... }
```

### 2. Protocol pack/parse と Physical I/O を分離する

プロトコルのフレーム構築・解析（pack/parse）と、物理バスへの送受信（I/O）は別ファイル・別クラスに分離する。これにより以下を実現する。

- プロトコル処理の単体テストが可能
- transport（物理層）を差し替え可能（例: シミュレーション用のモック）
- 安全レビューの範囲を限定できる

```
Protocol layer:
  protocols/RobStridePrivateProtocol.*   … frame pack/parse, ID helpers
  protocols/DdtM0602Protocol.*           … frame pack/parse, CRC-8/MAXIM
  protocols/Crc8Maxim.*                  … CRC計算

Transport layer:
  transport/CanTransportTwai.*           … ESP32-S3 TWAI 送受信
  transport/Rs485TransportUart.*         … UART + DE/RE 制御

Driver layer:
  drivers/CanEduLitePrivateDriver.*      … state, safety, target mapping
  drivers/Rs485DdtM0602Driver.*          … state, safety, target mapping
```

### 3. エラー条件は Fault 要因にする

以下の通信異常はすべて `Fault` 状態への遷移要因として扱う。

| 異常 | 対象バス | Fault 処理 |
|---|---|---|
| Timeout（応答なし） | CAN / RS485 | 一定時間応答がなければ Fault |
| Bus-off | CAN | TWAI が bus-off を報告したら Fault |
| CRC error | RS485 | CRC-8/MAXIM 不一致で Fault（リトライ超過時） |
| Invalid frame | CAN / RS485 | フレーム長不正、ID不一致、未知の通信タイプ |
| Error frame | CAN | CANエラーフレーム受信が頻発した場合 |

> [!NOTE]
> 単発のCRCエラーやtimeoutは即座にFaultとするか、リトライ後にFaultとするかは実装時に決定する。ただし、リトライ回数は定数化し、上限を設ける。

### 4. GPIO 割当は io_profile_matrix.md と整合させる

現時点の GPIO 割当状況:

| Transport | GPIO | Port | 状態 | 備考 |
|---|---|---|---|---|
| CAN_TX (TWAI) | G13 | PORT.A Yellow | Candidate | 外付けCAN transceiver必須 |
| CAN_RX (TWAI) | G15 | PORT.A White | Candidate | 外付けCAN transceiver必須 |
| RS485_TX (UART) | G2 | PORT.B Yellow | Candidate | MAKER-DRIVEと排他 |
| RS485_RX (UART) | G1 | PORT.B White | Candidate | MAKER-DRIVEと排他 |
| RS485 DE/RE | 未確定 | — | 未確定 | 追加GPIOが必要 |

> [!WARNING]
>
> - すべて Candidate 状態である。実機確認前に Provisional へ昇格させること。
> - RS485 の DE/RE 制御ピンは未確定。DinMeter の空きGPIOを調査する必要がある。
> - CAN (Port A) と RS485 (Port B) は物理的に異なるポートを使用するため、ハードウェア上は同時接続可能。ただし、ソフトウェア上はプロファイル単位で排他制御する。

---

## CAN Transport 候補: ESP32-S3 TWAI + 外付け CAN Transceiver

### アーキテクチャ

```
ESP32-S3 TWAI (内蔵CANコントローラ)
    ↓↑ GPIO (TX/RX)
外付け CAN transceiver (例: SN65HVD230, MCP2561 等)
    ↓↑ CAN_H / CAN_L
EDULITE 05
```

### 特徴

- ESP32-S3 は TWAI（Two-Wire Automotive Interface）として CAN 2.0 コントローラを内蔵
- 外付け transceiver で 3.3V ↔ CAN バスレベル変換を行う
- Extended frame (29-bit ID) 対応
- 1 Mbps 対応
- bus-off、エラーフレーム検出が可能

### 設計メモ

| 項目 | 方針 |
|---|---|
| ドライバ API | `twai_driver_install()`, `twai_start()`, `twai_transmit()`, `twai_receive()` |
| フィルタ | motor_id に基づくアクセプタンスフィルタを設定（実装時に決定） |
| タイミング | 1 Mbps 用の timing config を使用 |
| エラー処理 | bus-off → Fault 遷移、自動回復はしない |
| Timeout | 送信後一定時間内に応答がなければ Fault（値は定数化） |

### 未確定事項

- CAN transceiver の型番
- TWAI の timing config の最適値（ESP32-S3 の 1 Mbps では定義済みのはず）
- アクセプタンスフィルタの具体的な設定

---

## RS485 Transport 候補: UART + 外付け RS485 Transceiver + DE/RE 制御

### アーキテクチャ

```
ESP32-S3 UART
    ↓↑ GPIO (TX/RX)
外付け RS485 transceiver (例: MAX485, MAX3485 等)
    ↓ DE/RE 制御 ← ESP32-S3 GPIO
    ↓↑ A / B
DDT-M0602C234
```

### 特徴

- ESP32-S3 の UART を使用
- 外付け transceiver で 3.3V UART ↔ RS485 差動信号変換
- 半二重通信のため DE/RE 制御が必要
- 57600 bps / 8N1
- CRC-8/MAXIM による誤り検出

### DE/RE 制御

RS485 半二重通信では、送信時と受信時でトランシーバの方向を切り替える必要がある。

| 状態 | DE | /RE | バス方向 |
|---|---|---|---|
| 送信中 | HIGH | HIGH | ESP32 → バス |
| 受信中 | LOW | LOW | バス → ESP32 |

- `DE` と `/RE` を同一ピンで制御する場合、1本の GPIO で済む
- 別ピンの場合は 2本必要
- 制御ピンの割当は未確定

実際の端子名と論理はtransceiver型番に依存する。
`/RE` がactive lowであることをデータシートで確認し、`DE` と `/RE` を同一GPIOで制御する場合は HIGH=送信、LOW=受信となる構成を前提にする。

### 設計メモ

| 項目 | 方針 |
|---|---|
| UART API | `HardwareSerial` (Arduino) を使用 |
| DE/RE 切替 | 送信前に DE=H、送信完了後（flush 後）に DE=L |
| Timeout | 送信後 50ms 以内に 10バイト受信できなければ timeout |
| CRC | 送信時: DATA[0]〜DATA[8] から CRC 計算、受信時: CRC 検証 |
| エラー処理 | CRC不一致 → リトライ（上限あり）→ Fault |

### 未確定事項

- RS485 transceiver の型番
- DE/RE 制御ピン（DinMeter の空き GPIO 調査が必要）
- UART ポート番号（ESP32-S3 には複数 UART あり）
- 送信完了検出方法（`Serial.flush()` のタイミング）

---

## ファイル構成案

```
include/
  protocols/
    RobStridePrivateProtocol.h
    DdtM0602Protocol.h
    Crc8Maxim.h
  transport/
    CanTransportTwai.h
    Rs485TransportUart.h
  drivers/
    CanEduLitePrivateDriver.h
    Rs485DdtM0602Driver.h

src/
  protocols/
    RobStridePrivateProtocol.cpp
    DdtM0602Protocol.cpp
    Crc8Maxim.cpp
  transport/
    CanTransportTwai.cpp
    Rs485TransportUart.cpp
  drivers/
    CanEduLitePrivateDriver.cpp
    Rs485DdtM0602Driver.cpp
```

> [!NOTE]
> 上記は設計案であり、実装時に調整する。Arduino IDE 互換性（`embedded_cpp_policy.md`）を考慮し、ヘッダは `include/` 配下に配置する。

---

## 安全設計との関連

transport 層は、以下の安全設計と連携する。

| 安全要件 | transport の責任 |
|---|---|
| Disabled 状態では通信指令を送らない | Driver 層が制御。transport は送信関数を提供するだけ |
| Fault 時は停止指令を送る | Driver 層が stop/disable フレームを構築し、transport が送信する |
| Timeout で Fault 遷移 | transport が timeout を検出し、Driver に通知する |
| bus-off で Fault 遷移 | CAN transport が bus-off を検出し、Driver に通知する |
| CRC error で Fault 遷移 | RS485 transport が CRC 不一致を検出し、Driver に通知する |
| begin() で駆動しない | transport の初期化は通信路の確立のみ。Enable 送信は Driver の Armed 状態でのみ許可 |

---

## 禁止事項（本文書の範囲）

- コード変更を行わない
- Enable コマンド送信手順を実行手順として記載しない
- 速度指令、トルク指令、電流指令を初回手順に含めない
- EDULITE 05 / DDT-M0602C234 を採用済みと断定しない
- unknown の値を教材判断に使わない
- VAMeter で motor terminal measurement をしたと記述しない
- MAKER-DRIVE の試験結果を DD モータへ一般化しない
