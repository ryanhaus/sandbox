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

    logic [7:0] eeprom_read_values [16];

    initial begin
        eeprom_addr = 'b0;
        eeprom_din = 'b0;
        eeprom_wr_en = 'b0;
        ctl_parent_ready = 'b0;
        ctl_last = 'b0;

        #100

        /*
         * Test: Repeat the following 16 times:
         *  - Write 16 sequential bytes to EEPROM starting at address 0x123
         *  - Read back the 16 previous bytes
         *  - Verify values
         */
        for (int x = 0; x < 16; x++) begin
            // write EEPROM data address & first byte, and wait to finish
            eeprom_addr = { x[3:0], 7'h0 };
            eeprom_din = { x[3:0], 4'h0 };
            eeprom_wr_en = 'b1;
            ctl_parent_ready = 'b1;
            do #20; while(!ctl_module_ready);

            // write some more bytes out
            for (int i = 1; i < 16; i++) begin
                eeprom_din = { x[3:0], i[3:0] };
                ctl_last = (i == 15);
                do #20; while(!ctl_module_ready);
            end

            ctl_parent_ready = 'b0;
            ctl_last = 'b0;

            #20;

            // write EEPROM address with intention to read
            eeprom_addr = { x[3:0], 7'h0 };
            eeprom_wr_en = 'b0;
            ctl_parent_ready = 'b1;
            do #20; while(!ctl_module_ready);
            eeprom_read_values[0] = eeprom_dout;

            for (int i = 1; i < 16; i++) begin
                ctl_last = (i == 15);
                do #20; while(!ctl_module_ready);
                eeprom_read_values[i] = eeprom_dout;
            end

            ctl_parent_ready = 'b0;
            ctl_last = 'b0;

            // verify values
            for (int i = 0; i < 16; i++)
                if (eeprom_read_values[i] != { x[3:0], i[3:0] }) $error();

            #20;
        end
    end


endmodule
