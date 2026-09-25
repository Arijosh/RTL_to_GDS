`timescale 1ns/1ps

module jk_ff_tb;

    reg preset;   // Asynchronous reset (active high)
    reg clk;
    reg j, k;

    wire q, qn;

    // DUT instantiation
    jk_ff dut (
        .preset(preset),
        .clk(clk),
        .j(j),
        .k(k),
        .q(q),
        .qn(qn)
    );

    // Clock generation: 10 ns period
    always #5 clk = ~clk;

    // Monitor the input and output
    initial begin
        $monitor("%0t | preset=%b | j=%b | k=%b | q=%b | qn=%b",
                 $time, preset, j, k, q, qn);
    end

    initial begin
        // Initial values
        clk = 0;
        preset = 1;
        j = 0;
        k = 0;

        #7 preset = 0; // Deassert reset

        // Apply JK combinations
        #10 j = 0; k = 0;  // Hold
        #10 j = 0; k = 1;  // Reset
        #10 j = 1; k = 0;  // Set
        #10 j = 1; k = 1;  // Toggle
        #10 j = 1; k = 1;  // Toggle again

        // Assert asynchronous reset between clock edges
        #3 preset = 1;
        #4 preset = 0;

        #20 $finish;
    end

    // Dump waves
    initial begin
        $dumpfile("jk_ff_async.vcd");
        $dumpvars(0, jk_ff_tb);
    end

endmodule

