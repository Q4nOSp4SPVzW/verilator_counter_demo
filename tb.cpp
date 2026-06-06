#include "Vcounter.h"
#include "verilated.h"
#include "verilated_cov.h"
#include "verilated_vcd_c.h"

#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

static vluint64_t sim_time = 0;
static int checks = 0;
static uint8_t wave_test_id = 0;

struct Scoreboard {
    uint8_t expected = 0;

    void reset() {
        expected = 0;
    }

    void step(bool rst, bool en) {
        if (rst) {
            expected = 0;
        } else if (en) {
            expected = static_cast<uint8_t>((expected + 1U) & 0x0fU);
        }
    }
};

static void fail(const std::string& test, int cycle, int got, int expected) {
    std::cerr << "[FAIL] " << test
              << " cycle=" << cycle
              << " got=" << got
              << " expected=" << expected
              << std::endl;
    throw std::runtime_error(test + " failed");
}

static void check_count(Vcounter* top, Scoreboard& sb,
                        const std::string& test, int cycle) {
    ++checks;
    const int got = static_cast<int>(top->count);
    const int expected = static_cast<int>(sb.expected);
    if (got != expected) {
        fail(test, cycle, got, expected);
    }
}

static void tick(Vcounter* top, VerilatedVcdC* trace) {
    top->clk = 0;
    top->eval();
    if (trace) trace->dump(sim_time++);

    top->clk = 1;
    top->eval();
    if (trace) trace->dump(sim_time++);
}

static void apply_cycle(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb,
                        const std::string& test, int cycle, bool rst, bool en) {
    top->rst = rst;
    top->en = en;
    top->tb_test_id = wave_test_id;
    top->tb_cycle_in_test = static_cast<uint16_t>(cycle);
    sb.step(rst, en);
    tick(top, trace);
    check_count(top, sb, test, cycle);
}

static void initialize(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    wave_test_id = 0;
    top->clk = 0;
    top->rst = 1;
    top->en = 0;
    top->tb_test_id = wave_test_id;
    top->tb_cycle_in_test = 0;
    sb.reset();
    tick(top, trace);
    check_count(top, sb, "initial_reset", 0);
}

static void reset_test(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    std::cout << "[TEST] reset_test" << std::endl;
    wave_test_id = 1;
    for (int cycle = 0; cycle < 3; ++cycle) {
        apply_cycle(top, trace, sb, "reset_test", cycle, true, false);
    }
    for (int cycle = 3; cycle < 6; ++cycle) {
        apply_cycle(top, trace, sb, "reset_test", cycle, false, false);
    }
}

static void hold_test(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    std::cout << "[TEST] hold_test" << std::endl;
    wave_test_id = 2;
    apply_cycle(top, trace, sb, "hold_test", 0, false, true);
    apply_cycle(top, trace, sb, "hold_test", 1, false, true);
    for (int cycle = 2; cycle < 8; ++cycle) {
        apply_cycle(top, trace, sb, "hold_test", cycle, false, false);
    }
}

static void boundary_test(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    std::cout << "[TEST] boundary_test" << std::endl;
    wave_test_id = 3;
    apply_cycle(top, trace, sb, "boundary_test_reset", 0, true, false);
    for (int cycle = 1; cycle <= 20; ++cycle) {
        apply_cycle(top, trace, sb, "boundary_test", cycle, false, true);
        if (cycle >= 14 && cycle <= 17) {
            std::cout << "  cycle=" << cycle
                      << " count=" << static_cast<int>(top->count)
                      << std::endl;
        }
    }
}

static void reset_interrupt_test(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    std::cout << "[TEST] reset_interrupt_test" << std::endl;
    wave_test_id = 4;
    for (int cycle = 0; cycle < 6; ++cycle) {
        apply_cycle(top, trace, sb, "reset_interrupt_test", cycle, false, true);
    }
    apply_cycle(top, trace, sb, "reset_interrupt_test", 6, true, true);
    apply_cycle(top, trace, sb, "reset_interrupt_test", 7, false, true);
}

static void enable_toggle_test(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    std::cout << "[TEST] enable_toggle_test" << std::endl;
    wave_test_id = 5;
    const bool pattern[] = {true, false, true, true, false, false, true, false,
                            true, false, true, true, true, false, true, false};
    for (int cycle = 0; cycle < 16; ++cycle) {
        apply_cycle(top, trace, sb, "enable_toggle_test", cycle, false, pattern[cycle]);
    }
}

static void random_stress_test(Vcounter* top, VerilatedVcdC* trace, Scoreboard& sb) {
    std::cout << "[TEST] random_stress_test" << std::endl;
    wave_test_id = 6;
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> en_dist(0, 1);
    std::uniform_int_distribution<int> rst_dist(0, 19);

    for (int cycle = 0; cycle < 1000; ++cycle) {
        const bool rst = (rst_dist(gen) == 0);
        const bool en = (en_dist(gen) != 0);
        apply_cycle(top, trace, sb, "random_stress_test", cycle, rst, en);
    }
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vcounter* top = new Vcounter;
    VerilatedVcdC* trace = new VerilatedVcdC;
    Scoreboard sb;

    top->trace(trace, 99);
    trace->open("counter_trace.vcd");

    try {
        initialize(top, trace, sb);
        reset_test(top, trace, sb);
        hold_test(top, trace, sb);
        boundary_test(top, trace, sb);
        reset_interrupt_test(top, trace, sb);
        enable_toggle_test(top, trace, sb);
        random_stress_test(top, trace, sb);

        std::cout << "[PASS] all tests passed, checks=" << checks << std::endl;
    } catch (...) {
        VerilatedCov::write("coverage.dat");
        trace->close();
        top->final();
        delete trace;
        delete top;
        return 1;
    }

    VerilatedCov::write("coverage.dat");

    trace->close();
    top->final();
    delete trace;
    delete top;
    return 0;
}
