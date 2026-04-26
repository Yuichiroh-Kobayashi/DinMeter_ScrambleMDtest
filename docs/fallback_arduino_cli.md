# Fallback: Arduino CLI Implementation Plan

もしPlatformIOでのビルドやライブラリ解決が2時間以内に解決しない場合、以下の手順でArduino CLI（またはArduino IDE）へ移行します。

## 1. 前提条件
- Arduino CLI がインストールされていること、または VS Code の Arduino 拡張機能が使えること。
- `m5stack` のボードマネージャURLが登録されていること。
  - `https://m5stack.oss-cn-beijing.aliyuncs.com/resource/arduino/package_m5stack_index.json`

## 2. コア・ライブラリのインストール
```bash
arduino-cli core update-index
arduino-cli core install m5stack:esp32
arduino-cli lib install M5Unified
arduino-cli lib install M5GFX
arduino-cli lib install M5DinMeter
```

## 3. コンパイルと書き込み
ボード ID に `m5stack:esp32:stamps3` (または適切な ID) を指定します。

```bash
# コンパイル
arduino-cli compile --fqbn m5stack:esp32:stamps3 ./src/main.cpp

# 書き込み
arduino-cli upload -p COMx --fqbn m5stack:esp32:stamps3 ./src/main.cpp
```

## 4. 理由とメリット
- Arduino IDE/CLI は M5Stack 公式の推奨環境であり、ライブラリの依存関係がシンプルに解決できる場合が多い。
- `.ino` ファイルとして `src/main.cpp` をリネームするだけで移行可能。

## 5. 注意事項
- PlatformIOの `lib_deps` 管理機能が使えなくなるため、共有ライブラリのバージョン管理に注意が必要。
- `AppState.h` などの相対パス include は Arduino IDE の仕様に合わせる必要がある。
