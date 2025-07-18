module tca9539(
    input clk,     // Clock for simulation only, should be faster than SCL

    input reset_n, // Reset (active low)
    output int_n,  // Interrupt (active low)

    // I2C interface
    input scl,
    inout sda,

    // I2C address selection
    input a0, a1,

    // IO
    inout [15:0] port
);

    // Commands, see table 3 in datasheet
    typedef enum bit [2:0] {
        INPUT_PORT_0              = 3'b000,
        INPUT_PORT_1              = 3'b001,
        OUTPUT_PORT_0             = 3'b010,
        OUTPUT_PORT_1             = 3'b011,
        POLARITY_INVERSION_PORT_0 = 3'b100,
        POLARITY_INVERSION_PORT_1 = 3'b101,
        CONFIGURATION_PORT_0      = 3'b110,
        CONFIGURATION_PORT_1      = 3'b111
    } command;

    // Internal registers, table 3
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

    // Driver for 'port'
    generate
        for (genvar i = 0; i < 16; i++) begin
            assign port[i] = reg_configuration[i] ? 'bz : reg_output[i];
        end
    endgenerate

endmodule
