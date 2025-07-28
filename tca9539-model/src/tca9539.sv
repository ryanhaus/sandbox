module tca9539(
    input clk,     // Clock should be faster than SCL
    input reset_n, // Reset (active low)

    // I2C interface
    input scl,
    input sda_i,
    output sda_o,
    output sda_oe_n, // SDA enable (active low)
        

    // I2C address selection
    input [1:0] addr_sel, // sets bottom 2 bits of I2C address, base is 0x74

    // IO
    input [15:0] io_port_i,
    output [15:0] io_port_o,
    output [15:0] io_port_oe // signifies which bits in io_port_o are valid, active high
);

    wire rst = ~reset_n;

    // TCA9539 internal registers, table 3
    wire [7:0] input_port_0,
               input_port_1,
               output_port_0,
               output_port_1,
               polarity_inversion_port_0,
               polarity_inversion_port_1,
               configuration_port_0,
               configuration_port_1;

    // 8x 8-bit registers -> 4x 16-bit registers
    logic [15:0] reg_input;
    assign input_port_0 = reg_input[7:0];
    assign input_port_1 = reg_input[15:8];

    wire  [15:0] reg_output             = { output_port_1,             output_port_0             },
                 reg_polarity_inversion = { polarity_inversion_port_1, polarity_inversion_port_0 },
                 reg_configuration      = { configuration_port_1,      configuration_port_0      };

    // I2C slave
    i2cSlave_tca9539 i2c_slave (
        .clk(clk),
        .rst(rst),
        .sda_i(sda_i),
        .sda_o(sda_o),
        .sda_oe_n(sda_oe_n),
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

    // 'io_port_o', 'io_port_oe'
    assign io_port_o = reg_output ^ reg_polarity_inversion;
    assign io_port_oe = ~reg_configuration;

endmodule
