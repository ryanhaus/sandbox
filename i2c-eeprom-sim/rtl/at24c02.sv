module at24c02 #(
    parameter I2C_ADDR = 7'h50,
    parameter EEPROM_SIZE_BYTES = 2048
) (
    input clk,
    input rst,

    // I2C interface
    input scl_i,
    output scl_o,
    output scl_oe,

    input sda_i,
    output sda_o,
    output sda_oe
);

    logic [7:0] data [EEPROM_SIZE_BYTES];

    always_ff @(posedge clk) begin
        if (rst) begin
            // reset to all zeroes
            data <= '{default: 'b0};
        end
        else begin
            // TODO
        end
    end



    // I2C Slave
    logic [7:0] i2c_s_tdata,  i2c_m_tdata;
    logic       i2c_s_tvalid, i2c_m_tvalid,
                i2c_s_tready, i2c_m_tready,
                i2c_s_tlast,  i2c_m_tlast;

    logic [6:0] i2c_address;
    logic i2c_busy, i2c_active;
    
    i2c_slave i2c_slave_inst (
        .clk(clk),
        .rst(rst),
        .release_bus(i2c_release_bus),

        .s_axis_data_tdata(i2c_s_tdata),
        .s_axis_data_tvalid(i2c_s_tvalid),
        .s_axis_data_tready(i2c_s_tready),
        .s_axis_data_tlast(i2c_s_tlast),

        .m_axis_data_tdata(i2c_m_tdata),
        .m_axis_data_tvalid(i2c_m_tvalid),
        .m_axis_data_tready(i2c_m_tready),
        .m_axis_data_tlast(i2c_m_tlast),

        .scl_i(scl_i),
        .scl_o(scl_o),
        .scl_t(scl_oe),
        .sda_i(sda_i),
        .sda_o(sda_o),
        .sda_t(sda_oe),

        .busy(i2c_busy),
        .bus_address(i2c_address),
        .bus_addressed(),
        .bus_active(i2c_active),

        .enable('b1),
        .device_address(I2C_ADDR),
        .device_address_mask(7'h7F)
    );

endmodule
