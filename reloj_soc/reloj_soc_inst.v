	reloj_soc u0 (
		.audio_BCLK                                      (<connected-to-audio_BCLK>),                                      //                                     audio.BCLK
		.audio_DACDAT                                    (<connected-to-audio_DACDAT>),                                    //                                          .DACDAT
		.audio_DACLRCK                                   (<connected-to-audio_DACLRCK>),                                   //                                          .DACLRCK
		.buttons_export                                  (<connected-to-buttons_export>),                                  //                                   buttons.export
		.clk_clk                                         (<connected-to-clk_clk>),                                         //                                       clk.clk
		.config_SDAT                                     (<connected-to-config_SDAT>),                                     //                                    config.SDAT
		.config_SCLK                                     (<connected-to-config_SCLK>),                                     //                                          .SCLK
		.leds_export                                     (<connected-to-leds_export>),                                     //                                      leds.export
		.reset_reset_n                                   (<connected-to-reset_reset_n>),                                   //                                     reset.reset_n
		.video_vga_controller_0_external_interface_CLK   (<connected-to-video_vga_controller_0_external_interface_CLK>),   // video_vga_controller_0_external_interface.CLK
		.video_vga_controller_0_external_interface_HS    (<connected-to-video_vga_controller_0_external_interface_HS>),    //                                          .HS
		.video_vga_controller_0_external_interface_VS    (<connected-to-video_vga_controller_0_external_interface_VS>),    //                                          .VS
		.video_vga_controller_0_external_interface_BLANK (<connected-to-video_vga_controller_0_external_interface_BLANK>), //                                          .BLANK
		.video_vga_controller_0_external_interface_SYNC  (<connected-to-video_vga_controller_0_external_interface_SYNC>),  //                                          .SYNC
		.video_vga_controller_0_external_interface_R     (<connected-to-video_vga_controller_0_external_interface_R>),     //                                          .R
		.video_vga_controller_0_external_interface_G     (<connected-to-video_vga_controller_0_external_interface_G>),     //                                          .G
		.video_vga_controller_0_external_interface_B     (<connected-to-video_vga_controller_0_external_interface_B>)      //                                          .B
	);

