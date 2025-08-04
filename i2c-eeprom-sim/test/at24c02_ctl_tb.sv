module at24c02_ctl_tb();

    localparam AT_I2C_ADDR = 7'h50;

    logic clk, rst, at_scl_oe, at_sda_oe, ctl_scl_oe, ctl_sda_oe;
    wand scl, sda;

    logic [10:0] eeprom_addr;
    logic [7:0] eeprom_din, eeprom_dout;
    logic eeprom_wr_en, ctl_parent_ready, ctl_module_ready, ctl_last;

    at24c02 #(.I2C_ADDR(AT_I2C_ADDR)) at24c02_inst (
        .clk(clk),
        .rst(rst),

        .scl_i(scl),
        .scl_o(scl),
        .scl_oe(at_scl_oe),

        .sda_i(sda),
        .sda_o(sda),
        .sda_oe(at_sda_oe)
    );

    at24c02_ctl #(.SLAVE_ADDR(AT_I2C_ADDR)) ctl_inst (
        .clk(clk),
        .rst(rst),

        .address(eeprom_addr),
        .din(eeprom_din),
        .dout(eeprom_dout),
        .wr_en(eeprom_wr_en),
        .ready(ctl_module_ready),
        .parent_ready(ctl_parent_ready),
        .last(ctl_last),


        .scl_i(scl),
        .scl_o(scl),
        .scl_oe(ctl_scl_oe),

        .sda_i(sda),
        .sda_o(sda),
        .sda_oe(ctl_sda_oe)
    );


    initial begin
        // reset & clk forever
        clk = 1;
        rst = 1;

        #10
        clk = 0;

        #10
        clk = 1;

        #10
        clk = 0;
        rst = 0;

        forever
            #10 clk = ~clk;
    end

    initial begin
        eeprom_addr = 'b0;
        eeprom_din = 'b0;
        eeprom_wr_en = 'b0;
        ctl_parent_ready = 'b0;
        ctl_last = 'b0;

        #100

        // test 1: write to eeprom
        // write EEPROM data address & first byte, and wait to finish
        eeprom_addr = 'h123;
        eeprom_din = 'hF0;
        eeprom_wr_en = 'b1;
        ctl_parent_ready = 'b1;
        do #20; while(!ctl_module_ready);

        // write some more bytes out
        for (int i = 1; i < 15; i++) begin
            eeprom_din = { 4'hF, i[3:0] };
            do #20; while(!ctl_module_ready);
        end

        eeprom_din = 'hFF;
        ctl_last = 'b1;
        #20 ctl_parent_ready = 'b0;

        do #20; while(!ctl_module_ready);
    end


endmodule
