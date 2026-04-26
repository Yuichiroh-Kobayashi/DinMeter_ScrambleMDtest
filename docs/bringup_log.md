# Bring-up Log

## 2026-04-26

### Repository state

- 初期状態で確認できたファイルは `LICENSE` のみ。
- `git status --short` では `LICENSE` の変更と `.codex` が既に存在していたため、今回の作業では触れていない。

### Board selection

- M5Stack DinMeter v1.1はStamp-S3A / ESP32-S3を内蔵。
- M5Stack公式ドキュメントでDinMeter v1.1がPlatformIO対応であること、HOLDがGPIO46であること、エンコーダがGPIO40/GPIO41、ボタンがGPIO42であることを確認。
- PlatformIO公式ドキュメントで `m5stack-stamps3` board IDを確認。
- ローカル検索では `m5stack-stamps3.json` は見つからなかった。Windows側のPlatformIO packageが未取得、またはこのCodex環境からWindows側PlatformIO格納先を参照できていない可能性がある。
- DinMeter専用のPlatformIO board IDは未確定。見つかった場合は `m5stack-stamps3` より優先する。
- 今回は `board = m5stack-stamps3` を採用。理由はDinMeter v1.1のMCUがStamp-S3A / ESP32-S3であり、PlatformIOにM5Stack StampS3 board定義が存在するため。
- 参照:
  - https://docs.m5stack.com/en/core/DinMeter_v1.1
  - https://docs.platformio.org/en/latest/boards/espressif32/m5stack-stamps3.html

### Libraries

- `m5stack/M5DinMeter` を採用。
- `m5stack/M5Unified` と `m5stack/M5GFX` を明示的に `lib_deps` に追加。
- M5Stack Arduino exampleでは `#include "M5DinMeter.h"`、`DinMeter.begin(cfg, true)`、`DinMeter.Display`、`DinMeter.Encoder`、`DinMeter.BtnA` が使われている。

### Local verification limitation

- このCodex実行環境は `/mnt/c/...` のWSL側bashとして起動している。
- WSL側では `pio` が見つからなかった。
- Windows側PowerShellを `powershell.exe` 経由で起動しようとしたが、WSL interopの `UtilBindVsockAnyPort` エラーで実行できなかった。
- Windows側cmdを `cmd.exe` 経由で起動しようとしたが、同じWSL interopエラーで実行できなかった。
- 要件上WSLを使わないため、最終確認はWindows上のVS Code PlatformIO IDE TerminalまたはPowerShellで実施する。

### Failure classification notes

- board定義の問題:
  - DinMeter専用board IDは未確定。
  - `m5stack-stamps3` が使えない場合はPlatformIO espressif32 platformの更新、または `esp32-s3-devkitc-1` 相当への一時切替を検討する。
- ライブラリ解決の問題:
  - `m5stack/M5DinMeter` が解決できない場合はPlatformIO Library RegistryとGitHub `https://github.com/m5stack/M5DinMeter.git` の指定を確認する。
- include名の問題:
  - 現在は `#include <M5DinMeter.h>`。
  - Arduino exampleでは `#include "M5DinMeter.h"` も使われているため、必要なら引用符includeへ変更する。
- USB/COMポートの問題:
  - Windowsで `pio device list` を確認する。
  - 表示されない場合はUSBケーブル、USB Serial/JTAGドライバ、Device Managerを確認する。
- HOLD(GPIO46)未設定による電源保持失敗:
  - `setup()` の最初で `pinMode(46, OUTPUT)` と `digitalWrite(46, HIGH)` を実行している。
  - バッテリ駆動時にすぐ落ちる場合はHOLD設定が実行される前にリセットしていないか確認する。

### Test commands to run on Windows

```powershell
pio --version
pio project config
pio run
pio device list
pio run -t upload
pio device monitor

### Implementation (2026-04-26)

- **AppState.h**: `SystemState` enum (Disabled, Armed, Fault) を定義。
- **main.cpp**: 
  - `digitalWrite(46, HIGH)` による電源保持を実装。
  - `M5DinMeter` ライブラリを使用した UI 表示（タイトル、状態、ターゲット値）。
  - エンコーダによる `target` (-100 〜 +100) の操作。
  - 長押しによる `Armed` / `Disabled` 遷移を実装。
  - 安全のため、`target == 0` の時のみ `Armed` に遷移可能。
  - 実際のモータ出力は未実装（安全値固定）。

- エンコーダの感度:
  - `M5DinMeter` の `ENCODER` クラスは 1クリックで 4 または 2 カウント進む可能性があります。
  - 実機で `target` が飛びすぎる場合は、`main.cpp` で `diff / 4` のように調整が必要です。

- `platformio.ini` に `m5stack-stamps3` を設定済み。
- `pio run` でビルドを確認。
- 書き込みは `pio run -t upload` を使用する。
