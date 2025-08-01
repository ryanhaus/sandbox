module at24c02_ctl (
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
        .scl_oe(scl_t),
        .sda_i(sda_i),
        .sda_o(sda_o),
        .sda_oe(sda_t),

        .busy(),
        .bus_control(),
        .bus_active(),
        .missed_ack(),

        .prescale('d25),
        .stop_on_idle('b1)
    );

    // Control state machine
    int counter;

    always_ff @(posedge clk) begin
        if (rst) begin
            counter <= 0;
        end
        else begin
            if (cmd_ready && s_tready) begin
                counter <= counter + 1;
            end
        end
    end

    always_comb begin
        cmd_address = 'h50;
        cmd_start = 'b0;
        cmd_read = 'b0;
        cmd_write = 'b0;
        cmd_write_multiple = 'b0;
        cmd_stop = 'b0;
        cmd_valid = 'b0;
        cmd_ready = 'b0;

        s_tdata = 'b0;
        m_tvalid = 'b0;
        m_tready = 'b0;
        m_tlast = 'b0;

        case (counter)
            0: begin
                
            end
        endcase
    end

endmodule
