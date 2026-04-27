# Comments and Doxygen Policy

## Purpose

この文書は、ソースコード内コメント、Doxygenコメント、AI生成ドキュメントの役割分担を定める。

## Language

コメントは日本語で書く。

例外:
- 外部プロトコル名
- ライブラリAPI名
- CAN ID、register name、datasheet由来の英語
- Doxygenコマンド

## Comment Roles

| 種類 | 目的 |
|------|------|
| Doxygenコメント | ファイル、クラス、関数、構造体、API境界の説明 |
| 通常コメント | 安全上の理由、ハード制約、プロトコルトラップの説明 |
| Markdown文書 | 設計規約、取扱説明、検証手順、運用記録 |

## Doxygen Style

ファイル先頭には必要に応じてDoxygenコメントを書く。

```cpp
/**
 * @file main.cpp
 * @brief DinMeter簡易モータコントローラのBring-up用エントリポイント。
 *
 * 電源投入直後はDisabledで起動し、現段階では実モータ出力を行わない。
 */
```

関数コメントは、公開API、安全境界、物理出力を伴う関数に付ける。

```cpp
/**
 * @brief C620へ電流指令フレームを送信する。
 *
 * この関数はCANバスへ物理出力を行う。DisabledまたはFault状態では呼び出してはならない。
 *
 * @param currentCommandCount C620電流指令値。範囲は -16384 から +16384。
 */
void sendC620CurrentCommand(int16_t currentCommandCount);
```

## Required Doxygen Targets

次にはDoxygenコメントを付ける。
- ファイル先頭
- `MotorDriver` interface
- driver class
- physical output function
- protocol pack/parse function
- safety state transition function
- hardware pin definition block

## Do Not Comment Obvious Code

悪い例:

```cpp
// targetValueにdiffを足す
targetValue += diff;
```

良い例:

```cpp
// エンコーダは実機で1クリックあたり複数カウント進む可能性がある。
// 感度調整は実機確認後に行う。
targetPercent += encoderDiffCount;
```

## Safety Comments

安全仕様に関係するコメントは、理由を書く。

```cpp
// Disabled中に速度0制御を回すと、C620では制動電流が出る可能性がある。
// そのため、Disabledでは閉ループ制御を止め、電流指令0だけを許可する。
```

## AI Documentation Boundary

DoxygenコメントはAPIリファレンスの材料である。
エンドユーザー向け取扱説明書の代替ではない。

AIエージェントは、ユーザー操作や安全手順を変更した場合、以下も更新する。

- `docs/manuals/user_manual.md`
- `docs/manuals/quick_start.md`
- `docs/manuals/troubleshooting.md`

## Arduino IDE Compatibility

Arduino IDEへコピーする可能性があるため、Doxygenコメントは `.cpp`、`.h`、`.ino` のいずれでも読める標準的な `/** ... */` 形式を使う。

`@file` にファイル名を書く場合、PlatformIO版とArduino IDEエクスポート版で名前が変わる可能性がある。
移植時のズレを避ける場合は、次の形式を優先する。

```cpp
/**
 * @file
 * @brief DinMeter簡易モータコントローラのBring-up用エントリポイント。
 */
```
