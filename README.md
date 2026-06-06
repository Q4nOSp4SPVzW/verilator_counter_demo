# Verilator Counter Demo

[日本語](#日本語) / [English](#english)

---

## 日本語

VerilatorとC++テストベンチを使って、SystemVerilog RTLを検証するための小さな実践例です。

このデモでは4bitカウンタを検証し、以下を行います。

- C++テストベンチからRTLへ入力を与える
- C++側のスコアボードで期待値を計算する
- RTL出力と期待値を毎サイクル比較する
- カバレッジを出力する
- GTKWaveで見られるVCD波形を出力する
- 長い波形でも見やすいようにテスト番号信号を出す

### クイックスタート

UbuntuまたはWSL Ubuntuでは、以下を実行します。

```bash
sudo apt-get update
sudo apt-get install -y verilator lcov g++ gtkwave
git clone https://github.com/Q4nOSp4SPVzW/verilator_counter_demo.git
cd verilator_counter_demo
bash run_wsl.sh
```

成功すると、以下のように表示されます。

```text
[PASS] all tests passed, checks=1060
Total coverage (6/6) 100.00%
```

カバレッジHTMLを開く場合:

```bash
xdg-open html/index.html
```

波形を開く場合:

```bash
gtkwave counter_trace.gtkw
```

Windows + WSLの場合は、以下のバッチファイルも使えます。

```text
run_test.bat
view_wave.bat
open_tb_explanation.bat
```

### このデモで学べること

- `counter.sv` をVerilatorでC++シミュレーションモデルへ変換する方法
- `tb.cpp` からRTL入力を駆動する方法
- C++スコアボードでRTL出力を自動チェックする方法
- リセット、保持、境界値、途中リセット、enable切替、ランダムストレステストの書き方
- Verilatorのカバレッジ取得方法
- `genhtml` によるHTMLカバレッジレポート生成
- GTKWaveで波形を見る方法
- 長い波形を追いやすくするテスト番号信号の使い方

### ファイル構成

| ファイル | 内容 |
| --- | --- |
| `counter.sv` | 検証対象の4bitカウンタRTL |
| `tb.cpp` | スコアボード付きVerilator C++テストベンチ |
| `run_wsl.sh` | Linux/WSLでビルド、実行、カバレッジ生成を行うスクリプト |
| `run_test.bat` | Windowsから `run_wsl.sh` を実行するバッチ |
| `view_wave.bat` | WindowsからGTKWaveを開くバッチ |
| `counter_trace.gtkw` | GTKWave用の信号リスト |
| `tb_cpp_explanation.html` | `tb.cpp` の初心者向け解説 |
| `open_tb_explanation.bat` | WindowsでHTML解説を開くバッチ |

`obj_dir/`、`coverage.dat`、`coverage.info`、`html/`、`counter_trace.vcd` などの生成物はGit管理対象外です。

### 必要環境

LinuxまたはWSLに以下を入れます。

```bash
sudo apt-get update
sudo apt-get install -y verilator lcov g++ gtkwave
```

Windows用バッチファイルは、WSLのUbuntuディストリビューションを使う前提です。
WSL名が違う場合は、まず確認します。

```powershell
wsl -l -v
```

`Ubuntu-22.04` 以外の場合は、`run_test.bat` と `view_wave.bat` の以下を変更してください。

```bat
set "DISTRO=Ubuntu-22.04"
```

### テスト実行

LinuxまたはWSL:

```bash
cd verilator_counter_demo
bash run_wsl.sh
```

Windows:

```powershell
cd path\to\verilator_counter_demo
.\run_test.bat
```

期待される結果:

```text
[TEST] reset_test
[TEST] hold_test
[TEST] boundary_test
[TEST] reset_interrupt_test
[TEST] enable_toggle_test
[TEST] random_stress_test
[PASS] all tests passed, checks=1060
Total coverage (6/6) 100.00%
```

`checks=1060` は、C++スコアボードがRTL出力と期待値を1060回比較し、すべて一致したという意味です。

### テスト方針

`tb.cpp` はC++側に期待値モデルを持つスコアボード方式です。
各クロックで `rst` と `en` から期待される `count` を計算し、RTL出力 `count` と比較します。

期待動作:

| 条件 | 期待される `count` |
| --- | --- |
| `rst=1` | `0` |
| `rst=0, en=0` | 前回値を保持 |
| `rst=0, en=1` | 1増加 |
| `count=15, en=1` | `0` に戻る |

テスト内容:

| テスト | 確認内容 |
| --- | --- |
| `reset_test()` | リセットで0になり、解除後は保持されること |
| `hold_test()` | `en=0` で値を保持すること |
| `boundary_test()` | 4bitカウンタが `15` から `0` へ戻ること |
| `reset_interrupt_test()` | カウント中のリセットが優先されること |
| `enable_toggle_test()` | `en=1` のときだけカウントすること |
| `random_stress_test()` | 1000サイクルのランダムな `rst/en` 入力で期待値と一致すること |

ランダムテストは固定シードの `std::mt19937` を使っているため、失敗時も同じ入力列で再現できます。

### カバレッジ出力

成功後、以下が生成されます。

| 出力 | 内容 |
| --- | --- |
| `coverage.dat` | Verilatorの生カバレッジデータ |
| `coverage.info` | LCOV互換のカバレッジ情報 |
| `html/index.html` | HTMLカバレッジレポート |
| `annotated/` | Verilatorが注釈ファイルを出す場合の出力先 |

HTMLレポートを開く:

```bash
xdg-open html/index.html
```

Windows:

```powershell
Start-Process .\html\index.html
```

HTMLレポートでは `counter.sv` を開き、どのRTL行が実行されたかを確認します。
このデモでは100% line coverageになる想定です。

### 波形を見る

テストベンチは以下のVCDを出力します。

```text
counter_trace.vcd
```

GTKWaveで直接開く:

```bash
gtkwave counter_trace.vcd
```

信号リスト付きで開く:

```bash
gtkwave counter_trace.gtkw
```

Windows:

```powershell
.\view_wave.bat
```

シミュレーションを再実行してから波形を開く:

```powershell
.\view_wave.bat rebuild
```

見るとよい信号:

| 信号 | 意味 |
| --- | --- |
| `clk` | クロック |
| `rst` | 同期リセット |
| `en` | カウンタenable |
| `count[3:0]` | RTLカウンタ出力 |
| `tb_test_id[7:0]` | 現在のテスト番号 |
| `tb_cycle_in_test[15:0]` | 現在のテスト内サイクル番号 |

テスト番号:

| `tb_test_id` | テスト |
| --- | --- |
| `0` | 初期リセット |
| `1` | `reset_test()` |
| `2` | `hold_test()` |
| `3` | `boundary_test()` |
| `4` | `reset_interrupt_test()` |
| `5` | `enable_toggle_test()` |
| `6` | `random_stress_test()` |

波形が長い場合は、`tb_test_id` を見て目的のテスト区間へ移動します。
例えば `tb_test_id = 3` は `count` が `15` から `0` に戻る境界値テストです。

### 手動Verilatorコマンド

`run_wsl.sh` は、概ね以下のコマンドをまとめたものです。

```bash
verilator -Wall \
    --cc counter.sv \
    --exe tb.cpp \
    --coverage-line \
    --trace \
    --build

./obj_dir/Vcounter

verilator_coverage --annotate annotated coverage.dat
verilator_coverage --write-info coverage.info coverage.dat
genhtml --output-directory html --title "counter Verilator Coverage" --legend coverage.info
```

### 補足

- `tb_test_id` と `tb_cycle_in_test` は波形を見やすくするためのマーカーです。カウンタ本体の機能ではありません。
- `counter.sv` は `logic` や `always_ff` を使うため、SystemVerilogファイルとして `.sv` にしています。
- `VerilatedCov::write("coverage.dat")` でカバレッジを保存します。
- `--trace` と `VerilatedVcdC` でVCD波形を出力します。

### トラブルシュート

| 症状 | 確認すること |
| --- | --- |
| `verilator: command not found` | `sudo apt-get install -y verilator` を実行してください。 |
| `genhtml: command not found` | `sudo apt-get install -y lcov` を実行してください。 |
| `gtkwave: command not found` | `sudo apt-get install -y gtkwave` を実行してください。 |
| WindowsバッチでWSLが見つからない | `wsl -l -v` で名前を確認し、`.bat` の `DISTRO` を変更してください。 |
| 波形が出ない | 先に `bash run_wsl.sh` を実行し、`counter_trace.vcd` が生成されているか確認してください。 |
| テストが失敗する | `[FAIL]` 行を見てください。テスト名、サイクル、RTL値、期待値が表示されます。 |

---

## English

A small, practical Verilator example for learning how to test SystemVerilog RTL from a C++ testbench.

This demo verifies a 4-bit counter, writes coverage data, and generates a VCD waveform that can be opened with GTKWave.

### Quick Start

If you are using Ubuntu or WSL Ubuntu, run:

```bash
sudo apt-get update
sudo apt-get install -y verilator lcov g++ gtkwave
git clone https://github.com/Q4nOSp4SPVzW/verilator_counter_demo.git
cd verilator_counter_demo
bash run_wsl.sh
```

If the run succeeds, you should see:

```text
[PASS] all tests passed, checks=1060
Total coverage (6/6) 100.00%
```

Open the coverage report:

```bash
xdg-open html/index.html
```

Open the waveform:

```bash
gtkwave counter_trace.gtkw
```

On Windows with WSL, you can also double-click:

```text
run_test.bat
view_wave.bat
open_tb_explanation.bat
```

### What This Demonstrates

- Convert `counter.sv` into a C++ simulation model with Verilator
- Drive RTL inputs from `tb.cpp`
- Check RTL output against a C++ scoreboard every cycle
- Run reset, hold, boundary, reset-interrupt, enable-toggle, and random-stress tests
- Generate Verilator coverage data
- Generate an HTML coverage report with `genhtml`
- Generate a VCD waveform and open it with GTKWave
- Add test marker signals to make long waveforms easier to read

### Files

| File | Purpose |
| --- | --- |
| `counter.sv` | 4-bit counter RTL under test |
| `tb.cpp` | Verilator C++ testbench with scoreboard checks |
| `run_wsl.sh` | Linux/WSL script to build, run, and generate coverage |
| `run_test.bat` | Windows batch wrapper for `run_wsl.sh` |
| `view_wave.bat` | Windows batch file to open the waveform in GTKWave |
| `counter_trace.gtkw` | GTKWave save file with a useful signal list |
| `tb_cpp_explanation.html` | Beginner-friendly explanation of `tb.cpp` |
| `open_tb_explanation.bat` | Opens `tb_cpp_explanation.html` on Windows |

Generated files such as `obj_dir/`, `coverage.dat`, `coverage.info`, `html/`, and `counter_trace.vcd` are ignored by Git.

### Requirements

On Linux or WSL:

```bash
sudo apt-get update
sudo apt-get install -y verilator lcov g++ gtkwave
```

The Windows batch files assume WSL with an Ubuntu distribution. If your distribution name is different, edit the `DISTRO` variable in the `.bat` files.

Check your WSL distribution name:

```powershell
wsl -l -v
```

If the name is not `Ubuntu-22.04`, edit this line in `run_test.bat` and `view_wave.bat`:

```bat
set "DISTRO=Ubuntu-22.04"
```

### Run the Test

On Linux or WSL:

```bash
cd verilator_counter_demo
bash run_wsl.sh
```

On Windows:

```powershell
cd path\to\verilator_counter_demo
.\run_test.bat
```

Expected result:

```text
[TEST] reset_test
[TEST] hold_test
[TEST] boundary_test
[TEST] reset_interrupt_test
[TEST] enable_toggle_test
[TEST] random_stress_test
[PASS] all tests passed, checks=1060
Total coverage (6/6) 100.00%
```

The PASS line means the C++ scoreboard compared the RTL output with the expected value 1060 times and found no mismatch.

### Test Strategy

The C++ testbench uses a scoreboard. The scoreboard calculates the expected counter value from `rst` and `en`, then compares that value with the RTL output `count` after every clock.

Expected behavior:

| Condition | Expected `count` |
| --- | --- |
| `rst=1` | `0` |
| `rst=0, en=0` | Hold previous value |
| `rst=0, en=1` | Increment by 1 |
| `count=15, en=1` | Wrap to `0` |

Tests:

| Test | What it checks |
| --- | --- |
| `reset_test()` | Reset drives the counter to zero and release holds the value |
| `hold_test()` | `en=0` holds the current value |
| `boundary_test()` | 4-bit rollover from `15` to `0` |
| `reset_interrupt_test()` | Reset takes priority while counting |
| `enable_toggle_test()` | The counter increments only when `en=1` |
| `random_stress_test()` | 1000 cycles of reproducible random `rst/en` inputs |

The random test uses a fixed `std::mt19937` seed, so failures are reproducible.

### Coverage Output

After a successful run, the script generates:

| Output | Purpose |
| --- | --- |
| `coverage.dat` | Raw Verilator coverage data |
| `coverage.info` | LCOV-compatible coverage data |
| `html/index.html` | HTML coverage report |
| `annotated/` | Annotated RTL output when Verilator emits annotation files |

Open the HTML report:

```bash
xdg-open html/index.html
```

On Windows:

```powershell
Start-Process .\html\index.html
```

In the HTML report, open `counter.sv` to see which RTL lines were executed. This demo is expected to reach 100% line coverage.

### Waveform

The testbench writes:

```text
counter_trace.vcd
```

Open it directly with GTKWave:

```bash
gtkwave counter_trace.vcd
```

Or open it with the provided GTKWave signal list:

```bash
gtkwave counter_trace.gtkw
```

On Windows:

```powershell
.\view_wave.bat
```

Use `rebuild` to rerun the simulation before opening the waveform:

```powershell
.\view_wave.bat rebuild
```

Useful waveform signals:

| Signal | Meaning |
| --- | --- |
| `clk` | Clock |
| `rst` | Synchronous reset |
| `en` | Counter enable |
| `count[3:0]` | RTL counter output |
| `tb_test_id[7:0]` | Current test number |
| `tb_cycle_in_test[15:0]` | Cycle number inside the current test |

Test IDs:

| `tb_test_id` | Test |
| --- | --- |
| `0` | Initial reset |
| `1` | `reset_test()` |
| `2` | `hold_test()` |
| `3` | `boundary_test()` |
| `4` | `reset_interrupt_test()` |
| `5` | `enable_toggle_test()` |
| `6` | `random_stress_test()` |

When the waveform feels too long, use `tb_test_id` to jump to the section of interest. For example, `tb_test_id = 3` is the rollover test where `count` goes from `15` to `0`.

### Manual Verilator Commands

The script is just a wrapper around these commands:

```bash
verilator -Wall \
    --cc counter.sv \
    --exe tb.cpp \
    --coverage-line \
    --trace \
    --build

./obj_dir/Vcounter

verilator_coverage --annotate annotated coverage.dat
verilator_coverage --write-info coverage.info coverage.dat
genhtml --output-directory html --title "counter Verilator Coverage" --legend coverage.info
```

### Notes

- `tb_test_id` and `tb_cycle_in_test` are waveform markers. They are not part of the counter logic.
- The counter uses SystemVerilog syntax such as `logic` and `always_ff`, so the RTL file is named `counter.sv`.
- The testbench calls `VerilatedCov::write("coverage.dat")` to save coverage data.
- The VCD trace is enabled with `--trace` and `VerilatedVcdC`.

### Troubleshooting

| Symptom | What to check |
| --- | --- |
| `verilator: command not found` | Install Verilator with `sudo apt-get install -y verilator`. |
| `genhtml: command not found` | Install `lcov` with `sudo apt-get install -y lcov`. |
| `gtkwave: command not found` | Install GTKWave with `sudo apt-get install -y gtkwave`. |
| Windows batch says WSL distro was not found | Run `wsl -l -v` and update the `DISTRO` variable in the `.bat` files. |
| No waveform appears | Run `bash run_wsl.sh` first and confirm `counter_trace.vcd` was generated. |
| Test fails | Look at the `[FAIL]` line. It prints the test name, cycle, RTL value, and expected value. |
