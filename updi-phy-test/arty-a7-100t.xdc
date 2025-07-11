set_property -dict {PACKAGE_PIN E3 IOSTANDARD LVCMOS33} [get_ports clk]
create_clock -period 10.000 -name sys_clk_pin -waveform {0.000 5.000} -add [get_ports clk]

set_property -dict {PACKAGE_PIN C2 IOSTANDARD LVCMOS33} [get_ports rst]
set_property -dict {PACKAGE_PIN V15 IOSTANDARD LVCMOS33} [get_ports updi]

set_property PULLTYPE PULLUP [get_ports updi]
