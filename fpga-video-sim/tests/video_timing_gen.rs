use marlin::{
    verilator::{VerilatedModelConfig, VerilatorRuntime, VerilatorRuntimeOptions},
    verilog::prelude::*,
};
use snafu::Whatever;

#[verilog(src="src/rtl/video_timing_gen.sv", name="video_timing_gen")]
struct VideoTimingGen;

const MAX_SIM_TIME: u32 = 1_000_000;

#[test]
fn video_timing_gen() -> Result<(), Whatever> {
    let runtime = VerilatorRuntime::new(
        "artifacts".into(),
        &["src/rtl/video_timing_gen.sv".as_ref()],
        &["src/rtl".into()],
        [],
        VerilatorRuntimeOptions::default_logging(),
    )?;

    let mut main = runtime.create_model::<VideoTimingGen>(
        &VerilatedModelConfig {
            enable_tracing: true,
            ..Default::default()
        }
    )?;

    let mut vcd = main.open_vcd("video_timing_gen.vcd");

    let mut sim_time = 0;

    while sim_time < MAX_SIM_TIME {
        sim_time += 1;
    }

    Ok(())
}
