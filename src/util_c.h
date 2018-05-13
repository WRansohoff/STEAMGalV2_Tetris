#ifndef _VVC_UTIL_C_H
#define _VVC_UTIL_C_H

#include "global.h"
#include "sspi.h"
#include "peripherals.h"

// C-languages utility method signatures.

// Methods for interacting with specific I2C devices.
void ssd1306_start_sequence(I2C_TypeDef *I2Cx);
void ssd1331_start_sequence();

// Methods for writing to the 1KB OLED framebuffer.
// These don't actually write through to the screen until
// the 'i2c_display_framebuffer' method is called (see util.S)
void oled_draw_h_line(int x, int y, int w, uint8_t color);
void oled_draw_v_line(int x, int y, int h, uint8_t color);
void oled_draw_rect(int x, int y, int w, int h,
                    int outline, uint8_t color);
void oled_write_pixel(int x, int y, uint8_t color);
void oled_draw_letter(int x, int y, unsigned int w0, unsigned int w1, uint8_t color, char size);
void oled_draw_letter_c(int x, int y, char c, uint8_t color, char size);
void oled_draw_letter_i(int x, int y, int ic, uint8_t color, char size);
void oled_draw_text(int x, int y, char* cc, uint8_t color, char size);
void sspi_stream_framebuffer(void);

// Tetris methods!
void draw_main_menu(void);
void draw_game_over(void);
void draw_tetris_game(void);
void reset_game_state(void);
uint8_t check_brick_rot(int8_t new_r);
uint8_t check_brick_pos(int8_t xp, int8_t yp);
void tetris_clear_row(uint8_t row_num);
void tetris_game_tick(void);

#endif
