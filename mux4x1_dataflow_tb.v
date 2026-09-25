`timescale 1ns/1ps

module mux4x1_dataflow_tb;

    // Declare testbench variables
    reg [3:0] in;       // 4-bit input vector
    reg [1:0] sel;      // 2-bit selector

    wire out;          // Output from the multiplexer

    // Instantiate the Unit Under Test (UUT)
    mux4x1_dataflow uut (
        .in(in),
        .sel(sel),
        .out(out)
    );

    // Test stimulus block
    initial begin
        $display("DATAFLOW 4x1 MUX TEST");
        $display("----------------------");
        $display("SEL | IN   | OUT");
        $display("----------------------");

        // Set input values
        in = 4'b1010; // You can change this pattern to test more combinations

        // Apply all select line combinations with delay
        sel = 2'b00;
        #10 $display("%b | %b | %b", sel, in, out);

        sel = 2'b01;
        #10 $display("%b | %b | %b", sel, in, out);

        sel = 2'b10;
        #10 $display("%b | %b | %b", sel, in, out);

        sel = 2'b11;
        #10 $display("%b | %b | %b", sel, in, out);

        $finish; // End simulation
    end

    // Generate waveform file
    initial begin
        $dumpfile("mux4x1_dataflow.vcd");
        $dumpvars(0, mux4x1_dataflow_tb);
    end

endmodule