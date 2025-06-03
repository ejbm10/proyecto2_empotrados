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

    // Salidas VGA
    output wire vga_hs,
    output wire vga_vs,
    output wire vga_blank_n,
    output wire vga_sync_n,
    output wire [7:0] vga_r,
    output wire [7:0] vga_g,
    output wire [7:0] vga_b
);

    // =========================
    // Instancia del Qsys (reloj_soc)
    // =========================
    reloj_soc u0 (
        .audio_BCLK     (audio_BCLK),
        .audio_DACDAT   (audio_DACDAT),
        .audio_DACLRCK  (audio_DACLRCK),
        .config_SDAT    (config_SDAT),
        .config_SCLK    (config_SCLK),
        .buttons_export (buttons),
        .clk_clk        (clk),
        .leds_export    (leds),
        .reset_reset_n  (rst)
    );

    // =========================
    // Instancia del PLL para VGA
    // =========================
    wire vga_clk;

    reloj_soc_pll_0 pll_vga_inst (
        .refclk (clk),          // Clock de entrada (50 MHz)
        .rst    (~rst),         // Reset activo alto
        .outclk_0 (vga_clk),     // Clock de salida (25 MHz)
        .locked ()              // No usado por ahora
    );

    // =========================
    // Instancia del VGA Controller
    // =========================
    wire [9:0] pixel_x;
    wire [9:0] pixel_y;
    wire video_on;

    vga_controller vga_inst (
        .clk        (vga_clk),
        .reset      (~rst),
        .hsync      (vga_hs),
        .vsync      (vga_vs),
        .blank_b    (vga_blank_n),
        .sync_b     (vga_sync_n),
        .pixel_x    (pixel_x),
        .pixel_y    (pixel_y),
        .video_on   (video_on)
    );

    // =========================
    // Generación simple de color (ejemplo)
    // =========================
    assign vga_r = video_on ? 8'hFF : 8'h00;
    assign vga_g = video_on ? 8'h00 : 8'h00;
    assign vga_b = video_on ? 8'h00 : 8'h00;

endmodule

