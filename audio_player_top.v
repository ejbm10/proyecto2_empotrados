module audio_player_top (
    input wire clk,
    input wire rst,
    input wire [3:0] buttons,
    input wire audio_BCLK,
    input wire audio_DACLRCK,
    inout wire config_SDAT,
    output wire audio_DACDAT,
    output wire config_SCLK,
    output wire [27:0] segments,
    output wire [7:0] leds
);

    reloj_soc u0 (
        .audio_BCLK     (audio_BCLK),     
        .audio_DACDAT   (audio_DACDAT),   
        .audio_DACLRCK  (audio_DACLRCK),  
        .config_SDAT    (config_SDAT),    
        .config_SCLK    (config_SCLK),    
        .buttons_export (buttons), 
        .segments_export (segments),    
        .leds_export    (leds),    
        .clk_clk        (clk),        
        .reset_reset_n  (rst)   
    );  

endmodule
