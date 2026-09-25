`timescale 1ns/1ps

module half_adder_behavioral_tb;

// Testbench variables
    reg a, b;
    wire sum, carry;
	
// Instantiate the Unit Under Test
    half_adder_behavioral uut (
        .a(a),
        .b(b),
        .sum(sum),
        .carry(carry)
    );
	
// Stimulus block
    initial begin
        $display("Behavioral Half Adder Test");
        $display("A B | Sum Carry");
        $display("---------------");
		
// Apply all input combinations
        a = 0; b = 0;
        #10 $display("%b %b | %b %b", a, b, sum, carry);
a = 0; b = 1;
        #10 $display("%b %b | %b %b", a, b, sum, carry);
a = 1; b = 0;
        #10 $display("%b %b | %b %b", a, b, sum, carry);
a = 1; b = 1;
        #10 $display("%b %b | %b %b", a, b, sum, carry);
$finish;
    end
	
// Generate VCD waveform
    initial begin
        $dumpfile("half_adder_behavioral.vcd");
        $dumpvars(0, half_adder_behavioral_tb);
    end
endmodule
