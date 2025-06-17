	reloj_soc u0 (
		.audio_BCLK                                      (<connected-to-audio_BCLK>),                                      //                                     audio.BCLK
		.audio_DACDAT                                    (<connected-to-audio_DACDAT>),                                    //                                          .DACDAT
		.audio_DACLRCK                                   (<connected-to-audio_DACLRCK>),                                   //                                          .DACLRCK
		.audio_clock_clk                                 (<connected-to-audio_clock_clk>),                                 //                               audio_clock.clk
		.buttons_export                                  (<connected-to-buttons_export>),                                  //                                   buttons.export
		.clk_clk                                         (<connected-to-clk_clk>),                                         //                                       clk.clk
		.config_SDAT                                     (<connected-to-config_SDAT>),                                     //                                    config.SDAT
		.config_SCLK                                     (<connected-to-config_SCLK>),                                     //                                          .SCLK
		.memory_mem_a                                    (<connected-to-memory_mem_a>),                                    //                                    memory.mem_a
		.memory_mem_ba                                   (<connected-to-memory_mem_ba>),                                   //                                          .mem_ba
		.memory_mem_ck                                   (<connected-to-memory_mem_ck>),                                   //                                          .mem_ck
		.memory_mem_ck_n                                 (<connected-to-memory_mem_ck_n>),                                 //                                          .mem_ck_n
		.memory_mem_cke                                  (<connected-to-memory_mem_cke>),                                  //                                          .mem_cke
		.memory_mem_cs_n                                 (<connected-to-memory_mem_cs_n>),                                 //                                          .mem_cs_n
		.memory_mem_ras_n                                (<connected-to-memory_mem_ras_n>),                                //                                          .mem_ras_n
		.memory_mem_cas_n                                (<connected-to-memory_mem_cas_n>),                                //                                          .mem_cas_n
		.memory_mem_we_n                                 (<connected-to-memory_mem_we_n>),                                 //                                          .mem_we_n
		.memory_mem_reset_n                              (<connected-to-memory_mem_reset_n>),                              //                                          .mem_reset_n
		.memory_mem_dq                                   (<connected-to-memory_mem_dq>),                                   //                                          .mem_dq
		.memory_mem_dqs                                  (<connected-to-memory_mem_dqs>),                                  //                                          .mem_dqs
		.memory_mem_dqs_n                                (<connected-to-memory_mem_dqs_n>),                                //                                          .mem_dqs_n
		.memory_mem_odt                                  (<connected-to-memory_mem_odt>),                                  //                                          .mem_odt
		.memory_mem_dm                                   (<connected-to-memory_mem_dm>),                                   //                                          .mem_dm
		.memory_oct_rzqin                                (<connected-to-memory_oct_rzqin>),                                //                                          .oct_rzqin
		.reset_n_reset_n                                 (<connected-to-reset_n_reset_n>),                                 //                                   reset_n.reset_n
		.segments_export                                 (<connected-to-segments_export>),                                 //                                  segments.export
		.video_vga_controller_0_external_interface_CLK   (<connected-to-video_vga_controller_0_external_interface_CLK>),   // video_vga_controller_0_external_interface.CLK
		.video_vga_controller_0_external_interface_HS    (<connected-to-video_vga_controller_0_external_interface_HS>),    //                                          .HS
		.video_vga_controller_0_external_interface_VS    (<connected-to-video_vga_controller_0_external_interface_VS>),    //                                          .VS
		.video_vga_controller_0_external_interface_BLANK (<connected-to-video_vga_controller_0_external_interface_BLANK>), //                                          .BLANK
		.video_vga_controller_0_external_interface_SYNC  (<connected-to-video_vga_controller_0_external_interface_SYNC>),  //                                          .SYNC
		.video_vga_controller_0_external_interface_R     (<connected-to-video_vga_controller_0_external_interface_R>),     //                                          .R
		.video_vga_controller_0_external_interface_G     (<connected-to-video_vga_controller_0_external_interface_G>),     //                                          .G
		.video_vga_controller_0_external_interface_B     (<connected-to-video_vga_controller_0_external_interface_B>)      //                                          .B
	);

