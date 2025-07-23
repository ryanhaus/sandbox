module tca9539(
    input clk,     // Clock should be faster than SCL

    input reset_n, // Reset (active low)
    output logic int_n,  // Interrupt (active low)

    // I2C interface
    inout sda,
    inout scl,
        

    // I2C address selection
    input [1:0] addr_sel,

    // IO
    inout [15:0] port
);

    logic rst = ~reset_n;

    // I2C slave
    i2cSlave_tca9539 i2c_slave (
        .clk(clk),
        .rst(rst),
        .sda(sda),
        .scl(scl),
        .deviceAddress({ 5'b11101, addr_sel }), // From Table 2
        .myReg0(),
        .myReg1(),
        .myReg2(),
        .myReg3(),
        .myReg4(),
        .myReg5(),
        .myReg6(),
        .myReg7()
    );

    // TCA9539 commands, see table 3 in datasheet
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

    // 'reg_input'
    always_comb reg_input = port ^ reg_polarity_inversion;

    // 'port'
    logic [15:0] out_adj = reg_output ^ reg_polarity_inversion;

    generate
        for (genvar i = 0; i < 16; i++) begin
            // 1 -> input, 0 -> output
            assign port[i] = (reg_configuration[i] == 'b1) ? 'bz : out_adj[i];
        end
    endgenerate

    // 'int_n'
    always_comb begin
        int_n = 'b1;

        for (int i = 0; i < 16; i++) begin
            if (reg_configuration[i] == 'b1 && port[i] != reg_input[i]) begin
                int_n = 'b0;
            end
        end
    end

endmodule
