`timescale 1ns/1ps

module half_adder_behavioral(
    input a,
    input b,
    output reg sum,
    output reg carry
);

    // Combinational logic
    always @(a or b) begin
        sum = a ^ b;      // XOR for sum
        carry = a & b;    // AND for carry
    end

endmodule
