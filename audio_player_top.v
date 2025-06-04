module audio_player_top (
    input wire clk,                 // Reloj de sistema (50 MHz)
    input wire rst,                 // Reset
    input wire [3:0] buttons,
    input wire audio_BCLK,
    input wire audio_DACLRCK,
    inout wire config_SDAT,
    output wire audio_DACDAT,
    output wire config_SCLK,
    output wire [27:0] leds,

    // VGA
    output wire vga_clk,
    output wire vga_hs,
    output wire vga_vs,
    output wire vga_blank_n,
    output wire vga_sync_n,
    output wire [7:0] vga_r,
    output wire [7:0] vga_g,
    output wire [7:0] vga_b
);

    // Instancia del Qsys
    reloj_soc u0 (
        .audio_BCLK     (audio_BCLK),
        .audio_DACDAT   (audio_DACDAT),
        .audio_DACLRCK  (audio_DACLRCK),
        .buttons_export (buttons),
        .clk_clk        (clk),
        .config_SDAT    (config_SDAT),
        .config_SCLK    (config_SCLK),
        .leds_export    (leds),
        .reset_reset_n  (rst),

        // VGA - nombres EXACTOS de reloj_soc.v
        .video_vga_controller_0_external_interface_CLK   (vga_clk),
        .video_vga_controller_0_external_interface_HS    (vga_hs),
        .video_vga_controller_0_external_interface_VS    (vga_vs),
        .video_vga_controller_0_external_interface_BLANK (vga_blank_n),
        .video_vga_controller_0_external_interface_SYNC  (vga_sync_n),
        .video_vga_controller_0_external_interface_R     (vga_r),
        .video_vga_controller_0_external_interface_G     (vga_g),
        .video_vga_controller_0_external_interface_B     (vga_b)
    );

endmodule


