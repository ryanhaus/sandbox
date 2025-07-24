module tca9539(
    input clk,     // Clock should be faster than SCL
    input reset_n, // Reset (active low)

    // I2C interface
    input scl,
    input sda_i,
    output sda_o,
    output sda_o_en,
        

    // I2C address selection
    input [1:0] addr_sel,

    // IO
    input [15:0] io_port_i,
    output [15:0] io_port_o,
    output [15:0] io_port_o_en
);

    logic rst = ~reset_n;

    // TCA9539 internal registers, table 3
    logic [7:0] input_port_0,
                input_port_1,
                output_port_0,
                output_port_1,
                polarity_inversion_port_0,
                polarity_inversion_port_1,
                configuration_port_0,
                configuration_port_1;

    // 8x 8-bit registers -> 4x 16-bit registers
    logic [15:0] reg_input              = { input_port_1,              input_port_0              },
                 reg_output             = { output_port_1,             output_port_0             },
                 reg_polarity_inversion = { polarity_inversion_port_1, polarity_inversion_port_0 },
                 reg_configuration      = { configuration_port_1,      configuration_port_0      };

    // I2C slave
    i2cSlave_tca9539 i2c_slave (
        .clk(clk),
        .rst(rst),
        .sda_i(sda_i),
        .sda_o(sda_o),
        .sda_o_en(sda_o_en),
        .scl(scl),
        .deviceAddress({ 5'b11101, addr_sel }), // From Table 2
        .input_port_0(input_port_0),
        .input_port_1(input_port_1),
        .output_port_0(output_port_0),
        .output_port_1(output_port_1),
        .polarity_inversion_port_0(polarity_inversion_port_0),
        .polarity_inversion_port_1(polarity_inversion_port_1),
        .configuration_port_0(configuration_port_0),
        .configuration_port_1(configuration_port_1)
    );

    // 'reg_input'
    always_comb reg_input = io_port_i ^ reg_polarity_inversion;

    // 'io_port_o', 'io_port_o_en'
    assign io_port_o = reg_output ^ reg_polarity_inversion;
    assign io_port_o_en = reg_configuration;

endmodule
