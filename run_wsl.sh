#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

rm -rf obj_dir annotated html
rm -f coverage.dat coverage.info counter_trace.vcd

verilator -Wall \
    --cc counter.sv \
    --exe tb.cpp \
    --coverage-line \
    --trace \
    --build

./obj_dir/Vcounter

mkdir -p annotated
verilator_coverage --annotate annotated coverage.dat

if command -v genhtml >/dev/null 2>&1; then
    verilator_coverage --write-info coverage.info coverage.dat
    genhtml --output-directory html --title "counter Verilator Coverage" --legend coverage.info >/dev/null
fi

echo
echo "Generated:"
echo "  coverage.dat"
echo "  annotated/counter.sv"
echo "  counter_trace.vcd"
echo "  counter_trace.gtkw"
if [ -d html ]; then
    echo "  html/index.html"
fi
