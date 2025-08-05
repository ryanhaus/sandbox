use marlin::{
    verilator::{VerilatedModelConfig, VerilatorRuntime, VerilatorRuntimeOptions, AsVerilatedModel},
    verilator::vcd::Vcd,
    verilog::prelude::*,
};
use snafu::Whatever;

#[verilog(src="src/rtl/video_timing_gen.sv", name="video_timing_gen")]
struct VideoTimingGen;

const DOTCLK_DIV: usize = 10;
const PERIOD_HSYNC: usize = 10;
const PERIOD_HBP: usize = 20;
const PERIOD_HACTIVE: usize = 240;
const PERIOD_HFP: usize = 10;
const PERIOD_HORIZONTAL: usize = PERIOD_HSYNC + PERIOD_HBP + PERIOD_HACTIVE + PERIOD_HFP;
const PERIOD_VSYNC: usize = 2;
const PERIOD_VBP: usize = 2;
const PERIOD_VACTIVE: usize = 320;
const PERIOD_VFP: usize = 4;
const PERIOD_VERTICAL: usize = PERIOD_VSYNC + PERIOD_VBP + PERIOD_VACTIVE + PERIOD_VFP;

macro_rules! setup_runtime {
    () => {{
        VerilatorRuntime::new(
            "artifacts".into(),
            &["src/rtl/video_timing_gen.sv".as_ref()],
            &["src/rtl".into()],
            [],
            VerilatorRuntimeOptions::default_logging(),
        )?
    }};
}

macro_rules! reset_top_module {
    ($top:ident) => {
        $top.clk = 0;
        $top.rst = 1;
        $top.eval();

        $top.clk = 1;
        $top.eval();

        $top.clk = 0;
        $top.rst = 0;
    };
}

macro_rules! clock_cycle {
    ($top:ident) => {
        $top.clk = 0;
        $top.eval();

        $top.clk = 1;
        $top.eval();
    };
}

#[test]
fn check_dotclk_division() -> Result<(), Whatever> {
    let runtime = setup_runtime!();
    let mut top = runtime.create_model_simple::<VideoTimingGen>()?;
    reset_top_module!(top);

    for cycle in 0..5 {
        for i in 0..10 {
            let expected_dotclk = if i >= 5 { 0 } else { 1 };
            if top.dotclk != expected_dotclk { panic!(); }
            
            clock_cycle!(top);
        }
    }

    Ok(())
}

#[test]
fn check_hsync_period() -> Result<(), Whatever> {
    let runtime = setup_runtime!();
    let mut top = runtime.create_model_simple::<VideoTimingGen>()?;
    reset_top_module!(top);

    for cycle in 0..5 {
        let mut clks_0 = 0;
        let mut clks_1 = 0;

        while top.hsync == 0 {
            clks_0 += 1;
            clock_cycle!(top);
        }
        
        while top.hsync == 1 {
            clks_1 += 1;
            clock_cycle!(top);
        }

        clks_0 /= DOTCLK_DIV;
        clks_1 /= DOTCLK_DIV;

        if clks_0 != PERIOD_HSYNC { panic!(); }
        if clks_0 + clks_1 != PERIOD_HORIZONTAL { panic!(); }
    }

    Ok(())
}

#[test]
fn check_vsync_period() -> Result<(), Whatever> {
    let runtime = setup_runtime!();
    let mut top = runtime.create_model_simple::<VideoTimingGen>()?;
    reset_top_module!(top);

    for cycle in 0..5 {
        let mut clks_0 = 0;
        let mut clks_1 = 0;

        while top.vsync == 0 {
            clks_0 += 1;
            clock_cycle!(top);
        }
        
        while top.vsync == 1 {
            clks_1 += 1;
            clock_cycle!(top);
        }

        clks_0 /= (DOTCLK_DIV * PERIOD_HORIZONTAL);
        clks_1 /= (DOTCLK_DIV * PERIOD_HORIZONTAL);

        if clks_0 != PERIOD_VSYNC { panic!(); }
        if clks_0 + clks_1 != PERIOD_VERTICAL { panic!(); }
    }

    Ok(())
}
