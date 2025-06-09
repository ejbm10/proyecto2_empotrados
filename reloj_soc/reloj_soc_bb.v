
module reloj_soc (
	audio_BCLK,
	audio_DACDAT,
	audio_DACLRCK,
	buttons_export,
	clk_clk,
	config_SDAT,
	config_SCLK,
	reset_reset_n,
	segments_export,
	video_vga_controller_0_external_interface_CLK,
	video_vga_controller_0_external_interface_HS,
	video_vga_controller_0_external_interface_VS,
	video_vga_controller_0_external_interface_BLANK,
	video_vga_controller_0_external_interface_SYNC,
	video_vga_controller_0_external_interface_R,
	video_vga_controller_0_external_interface_G,
	video_vga_controller_0_external_interface_B);	

	input		audio_BCLK;
	output		audio_DACDAT;
	input		audio_DACLRCK;
	input	[3:0]	buttons_export;
	input		clk_clk;
	inout		config_SDAT;
	output		config_SCLK;
	input		reset_reset_n;
	output	[27:0]	segments_export;
	output		video_vga_controller_0_external_interface_CLK;
	output		video_vga_controller_0_external_interface_HS;
	output		video_vga_controller_0_external_interface_VS;
	output		video_vga_controller_0_external_interface_BLANK;
	output		video_vga_controller_0_external_interface_SYNC;
	output	[7:0]	video_vga_controller_0_external_interface_R;
	output	[7:0]	video_vga_controller_0_external_interface_G;
	output	[7:0]	video_vga_controller_0_external_interface_B;
endmodule
