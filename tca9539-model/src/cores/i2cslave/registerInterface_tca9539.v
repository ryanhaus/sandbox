//////////////////////////////////////////////////////////////////////
////                                                              ////
//// registerInterface.v                                          ////
////                                                              ////
//// This file is part of the i2cSlave opencores effort.
//// <http://www.opencores.org/cores//>                           ////
////                                                              ////
//// Module Description:                                          ////
//// You will need to modify this file to implement your 
//// interface.
//// Add your control and status bytes/bits to module inputs and outputs,
//// and also to the I2C read and write process blocks  
////                                                              ////
//// To Do:                                                       ////
//// 
////                                                              ////
//// Author(s):                                                   ////
//// - Steve Fielding, sfielding@base2designs.com                 ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2008 Steve Fielding and OPENCORES.ORG          ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE. See the GNU Lesser General Public License for more  ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from <http://www.opencores.org/lgpl.shtml>                   ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
`include "i2cSlave_define_tca9539.v"


module registerInterface_tca9539 (
  clk,
  rst,
  addr,
  dataIn,
  writeEn,
  dataOut,

  // registers (all 8-bit and r/w unless noted)
  input_port_0, // readonly
  input_port_1, // readonly
  output_port_0,
  output_port_1,
  polarity_inversion_port_0,
  polarity_inversion_port_1,
  configuration_port_0,
  configuration_port_1
);
input clk;
input rst;
input [7:0] addr;
input [7:0] dataIn;
input writeEn;
output [7:0] dataOut;
input  [7:0] input_port_0,              input_port_1;
output [7:0] output_port_0,             output_port_1,
             polarity_inversion_port_0, polarity_inversion_port_1,
             configuration_port_0,      configuration_port_1;

reg [7:0] dataOut;

reg [7:0] output_port_0,             output_port_1,
          polarity_inversion_port_0, polarity_inversion_port_1,
          configuration_port_0,      configuration_port_1;

// --- I2C Read
always @(posedge clk) begin
  case (addr)
    8'h00: dataOut <= input_port_0;
    8'h01: dataOut <= input_port_1;
    8'h02: dataOut <= output_port_0;
    8'h03: dataOut <= output_port_1;
    8'h04: dataOut <= polarity_inversion_port_0;
    8'h05: dataOut <= polarity_inversion_port_1;
    8'h06: dataOut <= configuration_port_0;
    8'h07: dataOut <= configuration_port_1;
    default: dataOut <= 8'h00;
  endcase
end

// --- I2C Write
always @(posedge clk) begin
  if (rst) begin
    output_port_0 = 'hFF;
    output_port_1 = 'hFF;
    polarity_inversion_port_0 = 'h00;
    polarity_inversion_port_1 = 'h00;
    configuration_port_0 = 'hFF;
    configuration_port_1 = 'hFF;
  end
  else if (writeEn == 1'b1) begin
    case (addr)
        8'h02: output_port_0 <= dataIn;
        8'h03: output_port_1 <= dataIn;
        8'h04: polarity_inversion_port_0 <= dataIn;
        8'h05: polarity_inversion_port_1 <= dataIn;
        8'h06: configuration_port_0 <= dataIn;
        8'h07: configuration_port_1 <= dataIn;
    endcase
  end
end

endmodule


 
