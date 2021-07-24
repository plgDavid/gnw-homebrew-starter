#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "main.h"
#include "gw_buttons.h"
#include "gw_lcd.h"
#include "gw_linker.h"
#include "bq24072.h"

uint32_t random_state = 0x1a3bb483;

static uint32_t rnd() {
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = random_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return random_state = x;
}

#define abs(x) ((x) < 0 ? -(x) : (x))

static void clear_screen() {
  pixel_t* framebuffer = lcd_get_active_buffer();
  memset(framebuffer, 0, GW_LCD_WIDTH * GW_LCD_HEIGHT * sizeof(pixel_t));
}

static void clear_both_framebuffers() {
  memset(framebuffer1, 0, GW_LCD_WIDTH * GW_LCD_HEIGHT * sizeof(pixel_t));
  memset(framebuffer2, 0, GW_LCD_WIDTH * GW_LCD_HEIGHT * sizeof(pixel_t));
}

static void draw_point(int32_t x, int32_t y, pixel_t color) {
  pixel_t* framebuffer = lcd_get_active_buffer();
  if (x >= 0 && x < GW_LCD_WIDTH && y >= 0 && y < GW_LCD_HEIGHT) framebuffer[y * GW_LCD_WIDTH + x] = color;
}

/*static void hline(int32_t x1, int32_t x2, int32_t y, pixel_t color) {
  while(x1 < x2) {
    draw_point(x1, y, color);
    ++x1;
  }
}

static void vline(int32_t x, int32_t y1, int32_t y2, pixel_t color) {
  while(y1 < y2) {
    draw_point(x, y1, color);
    ++y1;
  }
}*/

static void draw_rect(int32_t x, int32_t y, int32_t width, int32_t height, pixel_t color) {
  if (x < 0) {
    width += x;
    x = 0;
  }
  if (y < 0) {
    height += y;
    y = 0;
  }
  if (x + width > GW_LCD_WIDTH) width = GW_LCD_WIDTH - x;
  if (y + height > GW_LCD_HEIGHT) height = GW_LCD_HEIGHT - y;

  pixel_t* framebuffer = lcd_get_active_buffer();
  for (int32_t j = y; j < y + height; ++j) {
    pixel_t* pixel = &framebuffer[j * GW_LCD_WIDTH + x];
    for (int32_t i = 0; i < width; ++i) *(pixel++) = color;
  }
}

static void draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, pixel_t color) {
  int32_t dx =  abs(x1-x0);
  int32_t sx = x0 < x1 ? 1 : -1;
  int32_t dy = -abs(y1-y0);
  int32_t sy = y0<y1 ? 1 : -1;
  int32_t err = dx + dy;  /* error value e_xy */
  while (1) {   /* loop */
    draw_point(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    int32_t e2 = 2*err;
    if (e2 >= dy) { /* e_xy+e_x > 0 */
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) { /* e_xy+e_y < 0 */
      err += dx;
      y0 += sy;
    }
  }
}

static void draw_circle(int32_t xm, int32_t ym, int32_t r, pixel_t color) {
   int32_t x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
   do {
      draw_point(xm - x, ym + y, color); /*   I. Quadrant */
      draw_point(xm - y, ym - x, color); /*  II. Quadrant */
      draw_point(xm + x, ym - y, color); /* III. Quadrant */
      draw_point(xm + y, ym + x, color); /*  IV. Quadrant */
      r = err;
      if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
      if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
   } while (x < 0);
}

/*static void draw_char(int32_t x, int32_t y, u8 c, pixel_t color) {
  for (int32_t j = 0; j < CHAR_HEIGHT; j++) {
    u8 row = FONT[c * CHAR_HEIGHT + j];
    for (int32_t i = 0; i < CHAR_WIDTH; i++) {
      u8 bit = row & (1 << i);
      if (bit != 0) draw_point(x + i, y + j, color);
    }
  }
}

static void draw_text(int32_t x, int32_t y, const char* text, pixel_t color) {
  int32_t i = x;
  while(*text) {
    if (*text == '\n') {
      i = x;
      y += CHAR_HEIGHT;
    } else {
      draw_char(i, y, *text, color);
      i += CHAR_WIDTH;
    }
    text++;
  }
}*/

int mode = 7;
int frame = 0;

static void update() {

  char buffer[1024];
  char* battery_state[4] = {"missing", "charging", "discharging", "full"};
  snprintf(buffer, 1024, "%s\n\nFRAME=%d\nbuttons=%lx\nbat=%d%% %s\n\nA = BSOD\n<> = change demo\nPAUSE = reboot", __DATE__,frame, buttons_get(), bq24072_get_percent_filtered(), battery_state[bq24072_get_state()]);
  frame++;

  pixel_t* framebuffer = lcd_get_active_buffer();
  char c[2] = " ";
  switch (mode) {

	case 0:
      for( int i = 0; i < GW_LCD_WIDTH * GW_LCD_HEIGHT; i++) 
		  framebuffer[i] = rnd() % LCD_COLOR_MAX;
	break;

    case 1:
      for (int j = 0; j < 16; j++) {
        for (int i = 0; i < 16; i++) {
          c[0] = i + j * 16;
          lcd_draw_text_8x8(i * 8, j * 8, c, LCD_COLOR_WHITE);
          //draw_char(i * CHAR_WIDTH, j * CHAR_HEIGHT, i + j * 16, 0xffff);
        }
      }
      break;

    case 2:
      clear_screen();
      for (int i = 0; i < 100; i++)
        lcd_draw_text_8x8(rnd() % GW_LCD_WIDTH - 50, rnd() % GW_LCD_HEIGHT, "Hello world. This is some funny text!", rnd() % 65536);
        //draw_text(rnd() % WIDTH - 50, rnd() % HEIGHT, "Hello world. This is some funny text!", rnd() % 65536);
      break;

    case 3:
      clear_screen();
      for (int j = 0; j < GW_LCD_HEIGHT / 8; j++) {
        for (int i = 0; i < GW_LCD_WIDTH / 8; i++) {
          c[0] = rnd() % 96 + 32;
          lcd_draw_text_8x8(i * 8, j * 8, c, rnd() % LCD_COLOR_MAX);
          //draw_char(i * CHAR_WIDTH, j * CHAR_HEIGHT, rnd() % 256, rnd() % LCD_COLOR_MAX);
        }
      }
      break;

    case 4:
      for (int i = 0; i < 100; i++)
        draw_rect(rnd() % GW_LCD_WIDTH, rnd() % GW_LCD_HEIGHT, rnd() % 100 + 50, rnd() % 100 + 50, rnd() % LCD_COLOR_MAX);
      break;

    case 5:
      for (int i = 0; i < 100; i++)
        draw_line(rnd() % GW_LCD_WIDTH, rnd() % GW_LCD_HEIGHT, rnd() % GW_LCD_WIDTH, rnd() % GW_LCD_HEIGHT, rnd() % LCD_COLOR_MAX);
      break;

    case 6:
      for (int i = 0; i < 100; i++)
        draw_circle(rnd() % GW_LCD_WIDTH, rnd() % GW_LCD_HEIGHT, 20 + rnd() % 100, rnd() % LCD_COLOR_MAX);
          break;

    case 7:
      clear_screen();
      //draw_text(50, 50, buffer, 0x0f0f);
      lcd_draw_text_8x8(50, 50, buffer, LCD_COLOR_GREY);
      break;
  }
}

uint32_t button_state = 0;

int button_released(uint32_t button) {
  uint32_t buttons = buttons_get();
  int result = 0;
  if(buttons & button) {
    button_state |= button;
  } else {
    if(button_state & button) {
      result = 1;
    }
    button_state &= ~button;
  }
  return result;
}

void app_main() {

  lcd_backlight_set(255);
  memset(framebuffer1, 0x0, sizeof(framebuffer1));
  memset(framebuffer2, 0x0, sizeof(framebuffer2));

  int running = 1;
  while (running) {
    wdog_refresh();
    if (button_released(B_PAUSE)) { // quit (reboots if has nothing else to do)
      running = 0;
      break;
    } 
    if (button_released(B_POWER)) { // shutdown
      GW_EnterDeepSleep();
    }
    if (button_released(B_Right)) { // next demo
      clear_both_framebuffers();
      mode++; if(mode > 7) mode = 0;
    }
    if (button_released(B_Left)) { // prev demo
      clear_both_framebuffers();
      mode--; if(mode < 0) mode = 7;
    }
    if (button_released(B_A)) { // test BSOD
      abort();
    }
    update();
    lcd_swap();
    HAL_Delay(20);
  }
}
