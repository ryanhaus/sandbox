module video_timing_gen #(
    // how many clock pulses per dotclk pulse (should be even)
    parameter DOTCLK_DIV = 10,

    // horizontal timing parameters
    parameter PERIOD_HSYNC = 10,
    parameter PERIOD_HBP = 20,
    parameter PERIOD_HACTIVE = 240,
    parameter PERIOD_HFP = 10,

    // vertical timing parameters
    parameter PERIOD_VSYNC = 2,
    parameter PERIOD_VBP = 2,
    parameter PERIOD_VACTIVE = 320,
    parameter PERIOD_VFP = 4,

    // bit counts for the horizontal and vertical counters
    localparam PERIOD_HORIZONTAL = PERIOD_HSYNC + PERIOD_HBP + PERIOD_HACTIVE + PERIOD_HFP,
    localparam PERIOD_VERTICAL = PERIOD_VSYNC + PERIOD_VBP + PERIOD_VACTIVE + PERIOD_VFP,

    localparam CLK_DIV_CTR_BITS = $clog2(DOTCLK_DIV),
    parameter HORIZONTAL_CTR_BITS = $clog2(PERIOD_HORIZONTAL),
    parameter VERTICAL_CTR_BITS = $clog2(PERIOD_VERTICAL)
) (
    input clk,
    input rst,

    output logic dotclk,
    output logic hsync,
    output logic vsync,
    
    output logic [HORIZONTAL_CTR_BITS-1 : 0] ctr_h,
    output logic [VERTICAL_CTR_BITS-1 : 0] ctr_v
);

    // timing signal generation logic
    logic [CLK_DIV_CTR_BITS-1 : 0] ctr_clkdiv;
    logic [CLK_DIV_CTR_BITS-1 : 0] next_ctr_clkdiv;
    logic [HORIZONTAL_CTR_BITS-1 : 0] next_ctr_h;
    logic [VERTICAL_CTR_BITS-1 : 0] next_ctr_v;

    always_ff @(posedge clk) begin
        if (rst) begin
            ctr_clkdiv <= 'b0;
            ctr_h <= 'b0;
            ctr_v <= 'b0;
        end
        else begin
            ctr_clkdiv <= next_ctr_clkdiv;

            if (next_ctr_clkdiv == 'b0) begin
                ctr_h <= next_ctr_h;
                ctr_v <= next_ctr_v;
            end
        end
    end

    always_comb begin
        // calculate next ctr_clkdiv value
        next_ctr_clkdiv = ctr_clkdiv + 'b1;

        if (next_ctr_clkdiv == DOTCLK_DIV)
            next_ctr_clkdiv = 'b0;

        // figure out what dotclk should be
        dotclk = (ctr_clkdiv < DOTCLK_DIV / 2);

        // calculate next ctr_h & ctr_v values
        next_ctr_h = ctr_h + 'b1;
        next_ctr_v = ctr_v;

        if (next_ctr_h == PERIOD_HORIZONTAL) begin
            next_ctr_h = 'b0;
            next_ctr_v = ctr_v + 'b1;

            if (next_ctr_v == PERIOD_VERTICAL)
                next_ctr_v = 'b0;
        end

        // figure out what hsync and vsync should be
        hsync = (ctr_h >= PERIOD_HSYNC);
        vsync = (ctr_v >= PERIOD_VSYNC);
    end

endmodule
