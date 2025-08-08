from pathlib import Path

import cocotb
from cocotb.triggers import Timer
from cocotb.runner import get_runner

def test_video_timing_gen_runner():
    path = Path(__file__).resolve().parent.parent

    runner = get_runner("verilator")
    runner.build(
        verilog_sources = [path / "src/rtl/video_timing_gen.sv"],
        hdl_toplevel = "video_timing_gen",
        always = True,
    )

    runner.test(
        hdl_toplevel = "video_timing_gen",
        test_module = "test_video_timing_gen",
    )

@cocotb.test()
async def check_dotclk_division(dut):
    dut.clk.value = 0
    dut.rst.value = 1
    await Timer(10, units="ns")

    dut.clk.value = 1
    await Timer(10, units="ns")

    dut.clk.value = 0
    dut.rst.value = 0

    for cycle in range(5):
        for i in range(10):
            expected_dotclk = (i < 5);
            assert dut.dotclk.value == expected_dotclk
            
            dut.clk.value = 0
            await Timer(10, units="ns")

            dut.clk.value = 1
            await Timer(10, units="ns")
