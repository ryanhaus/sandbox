use marlin::{
    verilator::{VerilatedModelConfig, VerilatorRuntime, VerilatorRuntimeOptions, AsVerilatedModel},
    verilator::vcd::Vcd,
    verilog::prelude::*,
};
use snafu::Whatever;

#[verilog(src="src/rtl/video_timing_gen.sv", name="video_timing_gen")]
struct VideoTimingGen;

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

#[test]
fn check_dotclk_division() -> Result<(), Whatever> {
    let runtime = setup_runtime!();
    let mut top = runtime.create_model_simple::<VideoTimingGen>()?;
    reset_top_module!(top);

    for cycle in 0..5 {
        for i in 0..10 {
            let expected_dotclk = if i >= 5 { 0 } else { 1 };
            if top.dotclk != expected_dotclk { panic!(); }
            
            top.clk = 0;
            top.eval();

            top.clk = 1;
            top.eval();
        }
    }

    Ok(())
}
