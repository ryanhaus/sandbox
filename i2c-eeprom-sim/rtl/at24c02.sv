module at24c02 #(
    parameter I2C_ADDR = 7'h50
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

    // EEPROM signals
    logic [7:0] eeprom_bytes [2048];
    logic [10:0] eeprom_addr;
    logic [7:0] eeprom_din, eeprom_dout;
    logic eeprom_we;

    // I2C Slave signals
    logic [7:0] i2c_s_tdata, i2c_m_tdata;
    logic i2c_s_tvalid, i2c_m_tvalid,
          i2c_s_tready, i2c_m_tready,
          i2c_s_tlast, i2c_m_tlast;
    logic i2c_release_bus;



    // I2C to AXI stream
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

        .busy(),
        .bus_address(),
        .bus_addressed(),
        .bus_active(),

        .enable('b1),
        .device_address(I2C_ADDR),
        .device_address_mask(7'h7F)
    );



    // EEPROM handler
    always_ff @(posedge clk)
        if (eeprom_we)
            eeprom_bytes[eeprom_addr] <= eeprom_din;

    always_comb
        eeprom_dout = eeprom_bytes[eeprom_addr];



    // I2C state machine
    typedef enum {
        IDLE,
        RECV_ADDR_H,
        RECV_ADDR_L,
        ACTIVE
    } i2c_eeprom_state;

    i2c_eeprom_state state;

    always_ff @(posedge clk) begin
        if (rst) begin
            state <= IDLE;
        end
        else begin
            case (state)
                IDLE: if (i2c_m_tvalid) state <= RECV_ADDR_H;

                RECV_ADDR_H: begin
                    eeprom_addr[10:8] <= i2c_m_tdata[2:0];
                    state <= RECV_ADDR_L;
                end

                RECV_ADDR_L: begin
                    eeprom_addr[7:0] <= i2c_m_tdata;
                    state <= ACTIVE;
                end

                ACTIVE: if (i2c_m_tlast) state <= IDLE;
            endcase
        end
    end

    always_comb begin
        i2c_release_bus = 'b0;
        i2c_s_tdata = 'b0;
        i2c_s_tvalid = 'b0;
        i2c_m_tready = 'b1; // nothing ever blocks, so always ready
        i2c_s_tlast = 'b0;

        case (state)
            IDLE: i2c_release_bus = 'b1;

            ACTIVE: begin
                // for reads
                i2c_s_tdata = eeprom_dout;
                i2c_s_tvalid = 'b1;


                // for writes
                eeprom_din = i2c_m_tdata;
                eeprom_we = i2c_m_tvalid;
            end
        endcase
    end

endmodule
