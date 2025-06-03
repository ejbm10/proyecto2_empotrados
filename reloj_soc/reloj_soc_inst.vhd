	component reloj_soc is
		port (
			audio_BCLK      : in    std_logic                     := 'X';             -- BCLK
			audio_DACDAT    : out   std_logic;                                        -- DACDAT
			audio_DACLRCK   : in    std_logic                     := 'X';             -- DACLRCK
			buttons_export  : in    std_logic_vector(3 downto 0)  := (others => 'X'); -- export
			clk_clk         : in    std_logic                     := 'X';             -- clk
			config_SDAT     : inout std_logic                     := 'X';             -- SDAT
			config_SCLK     : out   std_logic;                                        -- SCLK
			leds_export     : out   std_logic_vector(7 downto 0);                     -- export
			reset_reset_n   : in    std_logic                     := 'X';             -- reset_n
			segments_export : out   std_logic_vector(27 downto 0)                     -- export
		);
	end component reloj_soc;

	u0 : component reloj_soc
		port map (
			audio_BCLK      => CONNECTED_TO_audio_BCLK,      --    audio.BCLK
			audio_DACDAT    => CONNECTED_TO_audio_DACDAT,    --         .DACDAT
			audio_DACLRCK   => CONNECTED_TO_audio_DACLRCK,   --         .DACLRCK
			buttons_export  => CONNECTED_TO_buttons_export,  --  buttons.export
			clk_clk         => CONNECTED_TO_clk_clk,         --      clk.clk
			config_SDAT     => CONNECTED_TO_config_SDAT,     --   config.SDAT
			config_SCLK     => CONNECTED_TO_config_SCLK,     --         .SCLK
			leds_export     => CONNECTED_TO_leds_export,     --     leds.export
			reset_reset_n   => CONNECTED_TO_reset_reset_n,   --    reset.reset_n
			segments_export => CONNECTED_TO_segments_export  -- segments.export
		);

