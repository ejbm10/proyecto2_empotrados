
module reloj_soc (
	audio_BCLK,
	audio_DACDAT,
	audio_DACLRCK,
	buttons_export,
	clk_clk,
	config_SDAT,
	config_SCLK,
	leds_export,
	reset_reset_n);	

	input		audio_BCLK;
	output		audio_DACDAT;
	input		audio_DACLRCK;
	input	[3:0]	buttons_export;
	input		clk_clk;
	inout		config_SDAT;
	output		config_SCLK;
	output	[27:0]	leds_export;
	input		reset_reset_n;
endmodule
