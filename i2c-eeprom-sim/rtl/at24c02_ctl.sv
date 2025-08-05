/*
 * Controller module for the AT24C02 I2C EEPROM chip.
 * Author: Ryan Hausmann
 *
 * This module is dependent on:
 *   - verilog-i2c: https://github.com/alexforencich/verilog-i2c (MIT license)
 *
 * See datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/doc0180.pdf
 *
 * Note that this module currently does not handle I2C bus errors.
 */
module at24c02_ctl # (
    parameter SLAVE_ADDR = 7'h50
) (
    input clk,
    input rst,

    // Control interface (AXIS-like)
    input [10:0] address,    // address of the first byte to read from/write to
    input wr_en,             // selects the direction of the tranfers, 0 = read, 1 = write
    input [7:0] din,         // input data (only valid during writes)
    output logic [7:0] dout, // output data (only valid during reads)
    output logic ready,      // signifies that this module is ready for a transfer to occur
    input parent_ready,      // signifies that parent module is ready for a transfer to occur
    input last,              // signifies that this transfer will be the last of this sequence

    // I2C interface
    input scl_i,
    output scl_o,
    output scl_oe,

    input sda_i,
    output sda_o,
    output sda_oe
);

    // Control signals
    logic [10:0] cur_address;
    logic cur_wr_en, cur_last;

    // I2C master signals
    logic [6:0] cmd_address;
    logic cmd_start,
          cmd_read,
          cmd_write,
          cmd_write_multiple,
          cmd_stop,
          cmd_valid,
          cmd_ready;

    logic [7:0] s_tdata, m_tdata;
    logic s_tvalid, m_tvalid,
          s_tready, m_tready,
          s_tlast, m_tlast;



    // I2C master
    i2c_master i2c_master_inst (
        .clk(clk),
        .rst(rst),
        
        .s_axis_cmd_address(cmd_address),
        .s_axis_cmd_start(cmd_start),
        .s_axis_cmd_read(cmd_read),
        .s_axis_cmd_write(cmd_write),
        .s_axis_cmd_write_multiple(cmd_write_multiple),
        .s_axis_cmd_stop(cmd_stop),
        .s_axis_cmd_valid(cmd_valid),
        .s_axis_cmd_ready(cmd_ready),

        .s_axis_data_tdata(s_tdata),
        .s_axis_data_tvalid(s_tvalid),
        .s_axis_data_tready(s_tready),
        .s_axis_data_tlast(s_tlast),

        .m_axis_data_tdata(m_tdata),
        .m_axis_data_tvalid(m_tvalid),
        .m_axis_data_tready(m_tready),
        .m_axis_data_tlast(m_tlast),

        .scl_i(scl_i),
        .scl_o(scl_o),
        .scl_t(scl_oe),
        .sda_i(sda_i),
        .sda_o(sda_o),
        .sda_t(sda_oe),

        .busy(),
        .bus_control(),
        .bus_active(),
        .missed_ack(),

        .prescale('d25),
        .stop_on_idle('b1)
    );

    // Control state machine
    typedef enum {
        IDLE,
        SETUP_WR_ADDR,
        WR_ADDR_H,
        WR_ADDR_L,
        WRITE,
        SETUP_RD,
        READ
    } ctl_state;

    ctl_state state;

    always_ff @(posedge clk) begin
        if (rst) begin
            state <= IDLE;
        end
        else begin
            case (state)
                // wait until ready signal
                IDLE:
                    if (parent_ready) begin
                        state <= SETUP_WR_ADDR;
                        cur_address <= address;
                        cur_wr_en <= wr_en;

                        // cur_last should be 0 on the first cycle to prevent
                        // immediately switching out of the READ/WRITE state
                        cur_last <= 'b0;
                    end

                // wait until a transfer has occured with I2C master ctrl if
                SETUP_WR_ADDR:
                    if (cmd_ready)
                        state <= WR_ADDR_H;

                // wait until a byte is able to be written out
                WR_ADDR_H:
                    if (s_tready)
                        state <= WR_ADDR_L;

                // wait until a byte is able to be written out
                WR_ADDR_L:
                    if (s_tready)
                        state <= cur_wr_en ? WRITE : READ;

                // loop until 'last' signal
                READ, WRITE: begin
                    // 'cur_last' is 'last' delayed by one cycle so
                    // that the last byte actually gets transferred
                    if (ready && parent_ready)
                        cur_last <= last;
                    // TODO: figure out a way to make it so that READ stays
                    // for long enough to send out cmd packet and data

                    if (cur_last)
                        state <= IDLE;
                end
            endcase
        end
    end

    always_comb begin
        // Module control interface
        ready = 'b0;
        dout = m_tdata;

        // I2C master command interface
        cmd_address = SLAVE_ADDR;
        cmd_start = 'b0;
        cmd_read = 'b0;
        cmd_write = 'b0;
        cmd_write_multiple = 'b0;
        cmd_stop = 'b0;
        cmd_valid = 'b0;

        // I2C master data interface
        s_tdata = 'b0;
        s_tvalid = 'b0;
        m_tready = 'b0;
        s_tlast = 'b0;

        case (state)
            IDLE: ready = 'b1;

            // write to command interface to expect a multiple byte write
            SETUP_WR_ADDR: begin
                cmd_start = 'b1;
                cmd_write_multiple = 'b1;
                cmd_valid = 'b1;

                // since writes are all one transaction, the stop flag needs
                // to be set here. for reads, there is a write transaction
                // (sending over address), then several read transactions, the
                // last read transaction should have the stop bit.
                if (cur_wr_en)
                    cmd_stop = 'b1;
            end

            // write the high byte of the EEPROM address out
            WR_ADDR_H: begin
                s_tdata = { 5'b0, cur_address[10:8] };
                s_tvalid = 'b1;
            end

            // write the low byte of the EEPROM address out
            WR_ADDR_L: begin
                s_tdata = cur_address[7:0];
                s_tvalid = 'b1;

                // if this is a read, this will be the last byte written
                if (!cur_wr_en)
                    s_tlast = 'b1;
            end

            // write out byte
            WRITE: begin
                s_tdata = din;
                s_tvalid = parent_ready;
                ready = s_tready;
                s_tlast = last;
            end

            // read in byte
            READ: begin
                cmd_read = 'b1;
                cmd_stop = last;
                cmd_valid = 'b1;

                m_tready = parent_ready;
                ready = m_tvalid;
            end
        endcase
    end

endmodule
