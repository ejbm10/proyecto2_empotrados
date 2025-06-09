	component reloj_soc is
		port (
			audio_BCLK                                      : in    std_logic                     := 'X';             -- BCLK
			audio_DACDAT                                    : out   std_logic;                                        -- DACDAT
			audio_DACLRCK                                   : in    std_logic                     := 'X';             -- DACLRCK
			buttons_export                                  : in    std_logic_vector(3 downto 0)  := (others => 'X'); -- export
			clk_clk                                         : in    std_logic                     := 'X';             -- clk
			config_SDAT                                     : inout std_logic                     := 'X';             -- SDAT
			config_SCLK                                     : out   std_logic;                                        -- SCLK
			reset_reset_n                                   : in    std_logic                     := 'X';             -- reset_n
			segments_export                                 : out   std_logic_vector(27 downto 0);                    -- export
			video_vga_controller_0_external_interface_CLK   : out   std_logic;                                        -- CLK
			video_vga_controller_0_external_interface_HS    : out   std_logic;                                        -- HS
			video_vga_controller_0_external_interface_VS    : out   std_logic;                                        -- VS
			video_vga_controller_0_external_interface_BLANK : out   std_logic;                                        -- BLANK
			video_vga_controller_0_external_interface_SYNC  : out   std_logic;                                        -- SYNC
			video_vga_controller_0_external_interface_R     : out   std_logic_vector(7 downto 0);                     -- R
			video_vga_controller_0_external_interface_G     : out   std_logic_vector(7 downto 0);                     -- G
			video_vga_controller_0_external_interface_B     : out   std_logic_vector(7 downto 0)                      -- B
		);
	end component reloj_soc;

	u0 : component reloj_soc
		port map (
			audio_BCLK                                      => CONNECTED_TO_audio_BCLK,                                      --                                     audio.BCLK
			audio_DACDAT                                    => CONNECTED_TO_audio_DACDAT,                                    --                                          .DACDAT
			audio_DACLRCK                                   => CONNECTED_TO_audio_DACLRCK,                                   --                                          .DACLRCK
			buttons_export                                  => CONNECTED_TO_buttons_export,                                  --                                   buttons.export
			clk_clk                                         => CONNECTED_TO_clk_clk,                                         --                                       clk.clk
			config_SDAT                                     => CONNECTED_TO_config_SDAT,                                     --                                    config.SDAT
			config_SCLK                                     => CONNECTED_TO_config_SCLK,                                     --                                          .SCLK
			reset_reset_n                                   => CONNECTED_TO_reset_reset_n,                                   --                                     reset.reset_n
			segments_export                                 => CONNECTED_TO_segments_export,                                 --                                  segments.export
			video_vga_controller_0_external_interface_CLK   => CONNECTED_TO_video_vga_controller_0_external_interface_CLK,   -- video_vga_controller_0_external_interface.CLK
			video_vga_controller_0_external_interface_HS    => CONNECTED_TO_video_vga_controller_0_external_interface_HS,    --                                          .HS
			video_vga_controller_0_external_interface_VS    => CONNECTED_TO_video_vga_controller_0_external_interface_VS,    --                                          .VS
			video_vga_controller_0_external_interface_BLANK => CONNECTED_TO_video_vga_controller_0_external_interface_BLANK, --                                          .BLANK
			video_vga_controller_0_external_interface_SYNC  => CONNECTED_TO_video_vga_controller_0_external_interface_SYNC,  --                                          .SYNC
			video_vga_controller_0_external_interface_R     => CONNECTED_TO_video_vga_controller_0_external_interface_R,     --                                          .R
			video_vga_controller_0_external_interface_G     => CONNECTED_TO_video_vga_controller_0_external_interface_G,     --                                          .G
			video_vga_controller_0_external_interface_B     => CONNECTED_TO_video_vga_controller_0_external_interface_B      --                                          .B
		);

