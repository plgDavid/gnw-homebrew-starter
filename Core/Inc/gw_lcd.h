#ifndef _LCD_H_
#define _LCD_H_

#include "stm32h7xx_hal.h"
#include <stdint.h>

#define GW_LCD_MODE_LUT8

#define GW_LCD_WIDTH  320
#define GW_LCD_HEIGHT 240


#ifdef GW_LCD_MODE_LUT8
	typedef uint8_t pixel_t;
	#define LCD_COLOR_MAX  0x100
	#define LCD_COLOR_MASK UINT8_MAX

	//TODO define these for EACH palette you need
	#define LCD_COLOR_WHITE  0x1
	#define LCD_COLOR_GREEN  0x2
	#define LCD_COLOR_BLUE   0x3//hmm
	#define LCD_COLOR_YELLOW 0x4//hmm
#else

	typedef uint16_t pixel_t;
	#define LCD_COLOR_MAX   0x10000
	#define LCD_COLOR_MASK  UINT16_MAX

	#define LCD_COLOR_WHITE  0xFFFF
	#define LCD_COLOR_GREEN  0x0f0f
	#define LCD_COLOR_BLUE   0x001F
	#define LCD_COLOR_YELLOW 0xF800
#endif // GW_LCD_MODE_LUT8

// 0 => framebuffer1
// 1 => framebuffer2
extern uint32_t active_framebuffer;

void lcd_deinit(SPI_HandleTypeDef *spi);
void lcd_init(SPI_HandleTypeDef *spi, LTDC_HandleTypeDef *ltdc);
void lcd_backlight_set(uint8_t brightness);
void lcd_backlight_on();
void lcd_backlight_off();
void lcd_swap(void);
void lcd_sync(void);

pixel_t* lcd_get_active_buffer  (void);
pixel_t* lcd_get_inactive_buffer(void);
void lcd_clear_both_framebuffers(void);

// To be used by fault handlers
void lcd_reset_active_buffer(void);
void lcd_draw_text_6x6(int x, int y, const char* text, pixel_t color);
void lcd_draw_text_8x8(int x, int y, const char* text, pixel_t color);

#endif
