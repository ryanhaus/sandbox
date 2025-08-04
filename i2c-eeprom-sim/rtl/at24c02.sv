/*
 * Simulation model of the AT24C02 I2C EEPROM chip.
 * Author: Ryan Hausmann
 *
 * This module is dependent on:
 *   - verilog-i2c: https://github.com/alexforencich/verilog-i2c (MIT license)
 *
 * See datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0180.pdf
 *
 * Limitations and differences from hardware:
 *   - Does not simulate pages, so any amount of bytes can be written to any
 *     address and it will not behave exactly like the hardware if the page
 *     boundaries are violated. See "PAGE WRITE" section in datasheet on pg
 *     9. TODO: simulate pages?
 *
 *   - The I2C address is configurable with the module parameter I2C_ADDR
 *     instead of using dedicated hardware pins.
 *
 *   - The write protection (WP) pin is omitted.
 *
 *   - There are no default values (although, this can be solved by using
 *     $readmemh or something to read from a file).
 */
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
    logic i2c_release_bus, i2c_addressed, i2c_bus_active;



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
        .bus_addressed(i2c_addressed),
        .bus_active(i2c_bus_active),

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
        READ,
        WRITE,
        STOP
    } i2c_eeprom_state;

    i2c_eeprom_state state;

    always_ff @(posedge clk) begin
        if (rst) begin
            state <= IDLE;
        end
        else begin
            case (state)
                IDLE:
                    if (i2c_addressed)
                        state <= RECV_ADDR_H;

                RECV_ADDR_H:
                    if (i2c_m_tvalid) begin
                        eeprom_addr[10:8] <= i2c_m_tdata[2:0];
                        state <= RECV_ADDR_L;
                    end

                RECV_ADDR_L:
                    if (i2c_m_tvalid) begin
                        eeprom_addr[7:0] <= i2c_m_tdata;

                        // i2c_m_tlast == 1 --> read operation (since no more
                        // bytes will be written)
                        state <= i2c_m_tlast ? READ : WRITE;
                    end

                READ:
                    if (!i2c_bus_active)
                        state <= STOP;
                    else if (i2c_s_tready)
                        // increase address pointer with every transaction
                        eeprom_addr <= eeprom_addr + 'b1;

                WRITE:
                    if (i2c_m_tlast)
                        state <= STOP;
                    else if (i2c_m_tvalid)
                        // increase address pointer with every transaction
                        eeprom_addr <= eeprom_addr + 'b1;

                STOP: state <= IDLE;
            endcase
        end
    end

    always_comb begin
        i2c_release_bus = 'b0;
        i2c_s_tdata = 'b0;
        i2c_s_tvalid = 'b0;
        i2c_m_tready = 'b1; // nothing ever blocks, so always ready
        i2c_s_tlast = 'b0;

        eeprom_din = 'b0;
        eeprom_we = 'b0;

        case (state)
            READ: begin
                i2c_s_tdata = eeprom_dout;
                i2c_s_tvalid = 'b1;
            end

            WRITE: begin
                eeprom_din = i2c_m_tdata;
                eeprom_we = i2c_m_tvalid;
            end
            
            STOP: i2c_release_bus = 'b1;
        endcase
    end

endmodule
