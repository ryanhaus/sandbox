use marlin::{
    verilator::{VerilatedModelConfig, VerilatorRuntime, VerilatorRuntimeOptions},
    verilog::prelude::*,
};
use snafu::Whatever;
use minifb::{Key, Window, WindowOptions};

#[verilog(src="src/rtl/top.sv", name="top")]
struct Top;

#[snafu::report]
fn main() -> Result<(), Whatever> {
    // set up simulation model
    let runtime = VerilatorRuntime::new(
        "artifacts".into(),
        &["src/rtl/top.sv".as_ref()],
        &["src/rtl".into()],
        [],
        VerilatorRuntimeOptions::default_logging(),
    )?;

    let mut top = runtime.create_model_simple::<Top>()?;

    // set up display
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

    const BUFFER_SIZE: usize = PERIOD_HACTIVE * PERIOD_VACTIVE;
    const WINDOW_SCALE: usize = 2;

    let mut vbuf: Vec<u32> = vec![0; BUFFER_SIZE];
    let mut window = Window::new(
        "FPGA Video Simulation",
        PERIOD_HACTIVE * WINDOW_SCALE,
        PERIOD_VACTIVE * WINDOW_SCALE,
        WindowOptions::default(),
    ).unwrap();

    window.set_target_fps(60);

    // run simulation + update display
    top.rst = 1;
    top.clk = 0;
    top.eval();

    top.clk = 1;
    top.eval();

    top.rst = 0;

    // clock until finished
    let mut prev_dotclk = top.dotclk; // for dotclk edge detection
    let mut h_ctr = 0;
    let mut v_ctr = 0;

    while window.is_open() && !window.is_key_down(Key::Escape) {
        let mut update_display = false;

        top.clk = if top.clk == 1 { 0 } else { 1 };
        top.eval();

        if top.dotclk == 1 && prev_dotclk == 0 {
            // posedge on dotclk, increment counters
            h_ctr += 1;

            if h_ctr == PERIOD_HORIZONTAL {
                h_ctr = 0;
                v_ctr += 1;

                if v_ctr == PERIOD_VERTICAL {
                    v_ctr = 0;
                    update_display = true;
                }
            }

            // if in active display region, update buffer
            if h_ctr >= PERIOD_HSYNC + PERIOD_HBP
                && h_ctr < PERIOD_HORIZONTAL - PERIOD_HFP
                && v_ctr >= PERIOD_VSYNC + PERIOD_VBP
                && v_ctr < PERIOD_VERTICAL - PERIOD_VFP {
                let h_pos = h_ctr - (PERIOD_HSYNC + PERIOD_HBP);
                let v_pos = v_ctr - (PERIOD_VSYNC + PERIOD_VBP);
                let i = h_pos + v_pos * PERIOD_HACTIVE;

                let (r, g, b) = (top.red as u32, top.green as u32, top.blue as u32);

                vbuf[i] = (r << 16) | (g << 8) | b;
            }

            // update the display, if necessary
            if update_display {
                window.update_with_buffer(&vbuf, PERIOD_HACTIVE, PERIOD_VACTIVE).unwrap();
            }
        }

        prev_dotclk = top.dotclk;
    }

    Ok(())
}
