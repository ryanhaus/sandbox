use marlin::{
    verilator::{VerilatorRuntime, VerilatorRuntimeOptions},
    verilog::prelude::*,
};

use snafu::Whatever;

#[verilog(src="src/rtl/top.sv", name="top")]
struct Top;

#[snafu::report]
fn main() -> Result<(), Whatever> {
    let runtime = VerilatorRuntime::new(
        "artifacts".into(),
        &["src/rtl/top.sv".as_ref()],
        &["src/rtl".into()],
        [],
        VerilatorRuntimeOptions::default_logging(),
    )?;

    let mut top = runtime.create_model_simple::<Top>()?;

    Ok(())
}
