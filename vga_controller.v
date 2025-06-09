module vga_controller(
    input wire clk,             // Reloj de entrada (25 MHz generado por el PLL)
    input wire reset,           // Señal de reset activo bajo
    output wire hsync,          // Señal de sincronización horizontal
    output wire vsync,          // Señal de sincronización vertical
    output wire blank_b,        // Señal para forzar negro fuera del área visible
    output wire sync_b,         // Sincronización combinada de hsync y vsync
    output wire [9:0] pixel_x,  // Coordenada X del píxel
    output wire [9:0] pixel_y,  // Coordenada Y del píxel
    output wire video_on        // Indica si estamos en el área visible
);

    // Parámetros de sincronización VGA 640x480 a 60 Hz
    localparam H_DISPLAY = 640;
    localparam H_FRONT_PORCH = 16;
    localparam H_SYNC_PULSE = 96;
    localparam H_BACK_PORCH = 48;
    localparam H_TOTAL = 800;

    localparam V_DISPLAY = 480;
    localparam V_FRONT_PORCH = 10;
    localparam V_SYNC_PULSE = 2;
    localparam V_BACK_PORCH = 33;
    localparam V_TOTAL = 525;

    // Región de la imagen (200x200) centrada en el área visible de 640x480
    localparam IMG_WIDTH = 200;
    localparam IMG_HEIGHT = 200;
    localparam H_IMG_START = (H_DISPLAY - IMG_WIDTH) / 2;
    localparam V_IMG_START = (V_DISPLAY - IMG_HEIGHT) / 2;

    // Contadores horizontales y verticales
    reg [9:0] h_count = 0;
    reg [9:0] v_count = 0;

    // Señales
    assign hsync = ~((h_count >= (H_DISPLAY + H_FRONT_PORCH)) && (h_count < (H_DISPLAY + H_FRONT_PORCH + H_SYNC_PULSE)));
    assign vsync = ~((v_count >= (V_DISPLAY + V_FRONT_PORCH)) && (v_count < (V_DISPLAY + V_FRONT_PORCH + V_SYNC_PULSE)));

    assign sync_b = hsync & vsync;

    assign video_on = (h_count >= H_IMG_START) && (h_count < H_IMG_START + IMG_WIDTH) &&
                      (v_count >= V_IMG_START) && (v_count < V_IMG_START + IMG_HEIGHT);

    assign blank_b = ~video_on;

    assign pixel_x = (h_count >= H_IMG_START && h_count < H_IMG_START + IMG_WIDTH) ? (h_count - H_IMG_START) : 10'd0;
    assign pixel_y = (v_count >= V_IMG_START && v_count < V_IMG_START + IMG_HEIGHT) ? (v_count - V_IMG_START) : 10'd0;

    // Contadores de píxeles
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            h_count <= 0;
            v_count <= 0;
        end else begin
            if (h_count == H_TOTAL - 1) begin
                h_count <= 0;
                if (v_count == V_TOTAL - 1) begin
                    v_count <= 0;
                end else begin
                    v_count <= v_count + 1;
                end
            end else begin
                h_count <= h_count + 1;
            end
        end
    end
endmodule
