`timescale 1ns/1ps

module audio_player_top_tb;

    reg clk;
    reg rst;
    reg [3:0] buttons;
    wire audio_BCLK;
    wire audio_DACLRCK;
    wire config_SDAT;
    wire audio_DACDAT;
    wire config_SCLK;
    wire [27:0] leds;

    // Instanciar el top
    audio_player_top dut (
        .clk(clk),
        .rst(rst),
        .buttons(buttons),
        .audio_BCLK(audio_BCLK),
        .audio_DACLRCK(audio_DACLRCK),
        .config_SDAT(config_SDAT),
        .audio_DACDAT(audio_DACDAT),
        .config_SCLK(config_SCLK),
        .leds(leds)
    );

    // Generador de clock (50 MHz)
    initial begin
        clk = 0;
        forever #10 clk = ~clk; // Periodo de 20 ns → 50 MHz
    end

    // Secuencia de reset y botones
    initial begin
        rst = 0;
        buttons = 4'b0000;
        #100;
        rst = 1; // Deassert reset
        #1000;

        // Simular botones
        buttons = 4'b0001; // Botón 0 presionado
        #50000;
        buttons = 4'b0000;
        #50000;

        buttons = 4'b0010; // Botón 1 presionado
        #50000;
        buttons = 4'b0000;
        #50000;

        // Terminar simulación
        #1000000;
        $finish;
    end

endmodule
