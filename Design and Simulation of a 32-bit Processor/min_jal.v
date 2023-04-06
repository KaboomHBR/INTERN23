module min_jal(OP, sig_jal);
	// min stands for Minterms
	// here we got just one term: sig_jal = OP[4]' AND OP[3]' AND ...
	input [4:0] OP;
	output sig_jal;
	
	and myOP_and (sig_jal, ~OP[4], ~OP[3], ~OP[2], OP[1], OP[0]);
	
endmodule
