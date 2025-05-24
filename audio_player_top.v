module audio_player_top (
	input wire clk,
	input wire rst,
	input wire [3:0] buttons,
	input wire audio_BCLK,
	input wire audio_DACLRCK,
	inout wire config_SDAT,
	output wire audio_DACDAT,
	output wire config_SCLK,
	output wire [27:0] leds
);

	reloj_soc u0 (
		.audio_BCLK     (audio_BCLK),     //   audio.BCLK
		.audio_DACDAT   (audio_DACDAT),   //        .DACDAT
		.audio_DACLRCK  (audio_DACLRCK),  //        .DACLRCK
		.config_SDAT    (config_SDAT),    //  config.SDAT
		.config_SCLK    (config_SCLK),    //        .SCLK
		.buttons_export (buttons), // buttons.export
		.clk_clk        (clk),        //     clk.clk
		.leds_export    (leds),    //    leds.export
		.reset_reset_n  (rst)   //   reset.reset_n
	);	

endmodule
