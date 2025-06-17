module audio_player_top (
    input wire clk,
    input wire rst,
    input wire [3:0] buttons,
    output wire audio_BCLK,
    output wire audio_DACLRCK,
    inout wire config_SDAT,
    output wire audio_DACDAT,
    output wire config_SCLK,
	 output wire audio_clk,
    output wire [27:0] segments,
	 output wire vga_clk,
    output wire vga_hs,
    output wire vga_vs,
    output wire vga_blank_n,
    output wire vga_sync_n,
    output wire [7:0] vga_r,
    output wire [7:0] vga_g,
    output wire [7:0] vga_b,
	 output wire [14:0] hps_ddr3_a,
	 output wire [2:0] hps_ddr3_ba,
	 output wire hps_ddr3_cas_n,
	 output wire hps_ddr3_cke,
	 output wire hps_ddr3_ck_n,
	 output wire hps_ddr3_ck_p,
	 output wire hps_ddr3_cs_n,
	 output wire [3:0] hps_ddr3_dm,
	 inout wire [31:0] hps_ddr3_dq,
	 inout wire [3:0] hps_ddr3_dqs_n,
	 inout wire [3:0] hps_ddr3_dqs_p,
	 output wire hps_ddr3_odt,
	 output wire hps_ddr3_ras_n,
	 output wire hps_ddr3_reset_n,
	 output wire hps_ddr3_we_n,
	 input wire hps_ddr3_rzq
); 
	 
	reloj_soc u0 (
		.audio_BCLK                                      (audio_BCLK),                                      //                                     audio.BCLK
		.audio_DACDAT                                    (audio_DACDAT),                                    //                                          .DACDAT
		.audio_DACLRCK                                   (audio_DACLRCK),                                   //                                          .DACLRCK
		.buttons_export                                  (buttons),                                  //                                   buttons.export
		.clk_clk                                         (clk),                                         //                                       clk.clk
		.config_SDAT                                     (config_SDAT),                                     //                                    config.SDAT
		.config_SCLK                                     (config_SCLK),                                     //                                          .SCLK
		.memory_mem_a                                    (hps_ddr3_a),                                    //                                    memory.mem_a
		.memory_mem_ba                                   (hps_ddr3_ba),                                   //                                          .mem_ba
		.memory_mem_ck                                   (hps_ddr3_ck_p),                                   //                                          .mem_ck
		.memory_mem_ck_n                                 (hps_ddr3_ck_n),                                 //                                          .mem_ck_n
		.memory_mem_cke                                  (hps_ddr3_cke),                                  //                                          .mem_cke
		.memory_mem_cs_n                                 (hps_ddr3_cs_n),                                 //                                          .mem_cs_n
		.memory_mem_ras_n                                (hps_ddr3_ras_n),                                //                                          .mem_ras_n
		.memory_mem_cas_n                                (hps_ddr3_cas_n),                                //                                          .mem_cas_n
		.memory_mem_we_n                                 (hps_ddr3_we_n),                                 //                                          .mem_we_n
		.memory_mem_reset_n                              (hps_ddr3_reset_n),                              //                                          .mem_reset_n
		.memory_mem_dq                                   (hps_ddr3_dq),                                   //                                          .mem_dq
		.memory_mem_dqs                                  (hps_ddr3_dqs_p),                                  //                                          .mem_dqs
		.memory_mem_dqs_n                                (hps_ddr3_dqs_n),                                //                                          .mem_dqs_n
		.memory_mem_odt                                  (hps_ddr3_odt),                                  //                                          .mem_odt
		.memory_mem_dm                                   (hps_ddr3_dm),                                   //                                          .mem_dm
		.memory_oct_rzqin                                (hps_ddr3_rzq),                                //                                          .oct_rzqin
		.reset_n_reset_n                                   (rst),                                   //                                     reset.reset_n
		.segments_export                                 (segments),                                 //                                  segments.export
		.video_vga_controller_0_external_interface_CLK   (vga_clk),   // video_vga_controller_0_external_interface.CLK
		.video_vga_controller_0_external_interface_HS    (vga_hs),    //                                          .HS
		.video_vga_controller_0_external_interface_VS    (vga_vs),    //                                          .VS
		.video_vga_controller_0_external_interface_BLANK (vga_blank_n), //                                          .BLANK
		.video_vga_controller_0_external_interface_SYNC  (vga_sync_n),  //                                          .SYNC
		.video_vga_controller_0_external_interface_R     (vga_r),     //                                          .R
		.video_vga_controller_0_external_interface_G     (vga_g),     //                                          .G
		.video_vga_controller_0_external_interface_B     (vga_b),      //                                          .B
		.audio_clock_clk                                 (audio_clk)                                  //                               audio_clock.clk
	);
	
endmodule


