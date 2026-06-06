module counter (
    input  logic       clk,
    input  logic       rst,
    input  logic       en,
    /* verilator lint_off UNUSED */
    input  logic [7:0] tb_test_id,
    input  logic [15:0] tb_cycle_in_test,
    /* verilator lint_on UNUSED */
    output logic [3:0] count
);
    always_ff @(posedge clk) begin
        if (rst) begin
            count <= 4'd0;
        end else if (en) begin
            count <= count + 4'd1;
        end
    end
endmodule
