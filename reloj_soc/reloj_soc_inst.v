	reloj_soc u0 (
		.audio_BCLK      (<connected-to-audio_BCLK>),      //    audio.BCLK
		.audio_DACDAT    (<connected-to-audio_DACDAT>),    //         .DACDAT
		.audio_DACLRCK   (<connected-to-audio_DACLRCK>),   //         .DACLRCK
		.buttons_export  (<connected-to-buttons_export>),  //  buttons.export
		.clk_clk         (<connected-to-clk_clk>),         //      clk.clk
		.config_SDAT     (<connected-to-config_SDAT>),     //   config.SDAT
		.config_SCLK     (<connected-to-config_SCLK>),     //         .SCLK
		.reset_reset_n   (<connected-to-reset_reset_n>),   //    reset.reset_n
		.segments_export (<connected-to-segments_export>)  // segments.export
	);

