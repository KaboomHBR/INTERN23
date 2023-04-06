`timescale 1 ps/ 1 ps

module utSke_tb();


   reg clock, reset;
	

	

	skeleton s_ut(clock, reset, imem_clock, dmem_clock, processor_clock, regfile_clock);
	
 	initial begin
        clock = 1'b0;
        reset = 1;
        
		  reset = 0;
		  #1 reset = 1;
			
		  #249 reset = 0;
 	
        //Now we have properly set up everything!



	end


	initial begin
		#6000 $stop;
		end
	
	always
       	#1 clock = ~clock; // toggle clock every 10 timescale units
 	
endmodule
