# Verilator Counter Demo

A small, practical Verilator example for learning how to test SystemVerilog RTL from a C++ testbench.

This demo verifies a 4-bit counter, writes coverage data, and generates a VCD waveform that can be opened with GTKWave.

## Quick Start

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

Then open the coverage report:

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

## What This Demonstrates

- Convert `counter.sv` into a C++ simulation model with Verilator
- Drive RTL inputs from `tb.cpp`
- Check RTL output against a C++ scoreboard every cycle
- Run reset, hold, boundary, reset-interrupt, enable-toggle, and random-stress tests
- Generate Verilator coverage data
- Generate an HTML coverage report with `genhtml`
- Generate a VCD waveform and open it with GTKWave
- Add test marker signals to make long waveforms easier to read

## Files

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

## Requirements

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

## Run the Test

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

## Test Strategy

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

## Coverage Output

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

## Waveform

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

## Manual Verilator Commands

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

## Notes

- `tb_test_id` and `tb_cycle_in_test` are waveform markers. They are not part of the counter logic.
- The counter uses SystemVerilog syntax such as `logic` and `always_ff`, so the RTL file is named `counter.sv`.
- The testbench calls `VerilatedCov::write("coverage.dat")` to save coverage data.
- The VCD trace is enabled with `--trace` and `VerilatedVcdC`.

## Troubleshooting

| Symptom | What to check |
| --- | --- |
| `verilator: command not found` | Install Verilator with `sudo apt-get install -y verilator`. |
| `genhtml: command not found` | Install `lcov` with `sudo apt-get install -y lcov`. |
| `gtkwave: command not found` | Install GTKWave with `sudo apt-get install -y gtkwave`. |
| Windows batch says WSL distro was not found | Run `wsl -l -v` and update the `DISTRO` variable in the `.bat` files. |
| No waveform appears | Run `bash run_wsl.sh` first and confirm `counter_trace.vcd` was generated. |
| Test fails | Look at the `[FAIL]` line. It prints the test name, cycle, RTL value, and expected value. |
