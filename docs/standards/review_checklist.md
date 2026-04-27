
# Review Checklist

## Purpose

このチェックリストは、人間およびAIエージェントが差分レビューを行うときに使用する。

## 1. Safety

- [ ] 電源投入直後は `Disabled` のままか。
- [ ] 起動時 `target = 0` が維持されているか。
- [ ] `Disabled` で物理出力が出ないか。
- [ ] `Fault` で物理出力が出ないか。
- [ ] `Armed` へは長押し、かつ `target == 0` のときだけ遷移するか。
- [ ] 実モータ出力を追加した場合、検証手順と撤退条件があるか。
- [ ] 実モータ出力追加が、無関係なUI変更や整理変更と混ざっていないか。

## 2. Arduino / PlatformIO Compatibility

- [ ] `setup()` / `loop()` が維持されているか。
- [ ] PlatformIO専用APIへ不要に依存していないか。
- [ ] Arduino IDEへ移植しにくいinclude構成になっていないか。
- [ ] `platformio.ini` の変更理由が説明されているか。
- [ ] ライブラリ依存がREADMEまたはdocsに反映されているか。

## 3. Naming / Units

- [ ] 物理量に単位が入っているか。
- [ ] `value`, `data`, `tmp`, `flag` などが不必要に使われていないか。
- [ ] 状態名が `Disabled / Armed / Fault` に統一されているか。
- [ ] 物理出力を伴う関数名に副作用が明示されているか。

## 4. Architecture

- [ ] UI処理とSafety処理が混ざっていないか。
- [ ] Protocol pack/parseと物理I/Oが分離されているか。
- [ ] Driver classが `driver_interface.md` と矛盾していないか。
- [ ] `begin()` やconstructorで物理出力していないか。
- [ ] `setTargetPercent()` が直接ハード出力していないか。

## 5. Comments / Documentation

- [ ] ファイル先頭に必要なDoxygenコメントがあるか。
- [ ] コメントは日本語か。
- [ ] コメントが「何をしているか」ではなく「なぜ必要か」を説明しているか。
- [ ] ユーザー操作・表示・配線・安全仕様を変えた場合、取扱説明書も更新されているか。
- [ ] 未実装機能を使えるように書いていないか。

## 6. Hardware Docs

- [ ] 対象デバイスの `docs/hardware/*/spec.md` を参照しているか。
- [ ] 未確認仕様を断定していないか。
- [ ] 電圧、電流、通信速度、CAN ID、ボーレート、スケーリングが明示されているか。
- [ ] 実機検証が必要な項目を `未検証` として残しているか。

## 7. Build / Test

- [ ] `pio run` が通るか。
- [ ] 書き込み手順に変更がある場合、READMEまたはoperations docsが更新されているか。
- [ ] 実機検証した場合、hardware test logに記録されているか。
- [ ] 失敗時の切り戻し方法があるか。
