module tca9539(
	input reset_n, // Reset (active low)
	output int_n,  // Interrupt (active low)

	// I2C interface
	input scl,
	inout sda,

	// I2C address selection
	input a0, a1

	// GPIOs (note: only [7:0] and [17:10] are valid, [9:8] are not driven)
	inout [17:0] port
);

endmodule
