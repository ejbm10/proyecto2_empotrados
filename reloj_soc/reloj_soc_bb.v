
module reloj_soc (
	audio_BCLK,
	audio_DACDAT,
	audio_DACLRCK,
	buttons_export,
	clk_clk,
	config_SDAT,
	config_SCLK,
	reset_reset_n,
	segments_export);	

	input		audio_BCLK;
	output		audio_DACDAT;
	input		audio_DACLRCK;
	input	[3:0]	buttons_export;
	input		clk_clk;
	inout		config_SDAT;
	output		config_SCLK;
	input		reset_reset_n;
	output	[27:0]	segments_export;
endmodule
