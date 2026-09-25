//timescale Mux Dataflow modelling

`timescale 1ns/1ps

module mux4x1_dataflow(
    input [3:0] in,
    input [1:0] sel,
    output out
);

assign out = (~sel[1] & ~sel[0] & in[0]) |
             (~sel[1] &  sel[0] & in[1]) |
             ( sel[1] & ~sel[0] & in[2]) |
             ( sel[1] &  sel[0] & in[3]);

endmodule