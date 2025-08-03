module top(
    input clk,
    input rst,

    output dotclk,
    output hsync,
    output vsync,
    output [7:0] red,
    output [7:0] green,
    output [7:0] blue
);

    // generate timing signals (default timing parameters for ILI9341)
    localparam CTR_H_BITS = timing_gen_inst.HORIZONTAL_CTR_BITS;
    localparam CTR_V_BITS = timing_gen_inst.VERTICAL_CTR_BITS;

    logic [CTR_H_BITS-1 : 0] display_pos_h, display_pos_v;
    logic in_display_region;

    video_timing_gen timing_gen_inst (
        .clk(clk),
        .rst(rst),
        .dotclk(dotclk),
        .hsync(hsync),
        .vsync(vsync),
        .pos_h(display_pos_h),
        .pos_v(display_pos_v),
        .active(in_display_region)
    );

    // generate video
    red = display_pos_h[7:0];
    green = display_pos_v[7:0];
    blue = 8'h80;

endmodule
