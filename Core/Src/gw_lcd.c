#include <string.h>

#include "gw_lcd.h"
#include "stm32h7xx_hal.h"
#include "main.h"

#if GW_LCD_MODE_LUT8
uint8_t framebuffer1[GW_LCD_WIDTH * GW_LCD_HEIGHT];
uint8_t framebuffer2[GW_LCD_WIDTH * GW_LCD_HEIGHT];
#else
uint16_t framebuffer1[GW_LCD_WIDTH * GW_LCD_HEIGHT];
uint16_t framebuffer2[GW_LCD_WIDTH * GW_LCD_HEIGHT];
#endif // GW_LCD_MODE_LUT8

extern LTDC_HandleTypeDef hltdc;

extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;

uint32_t active_framebuffer;

void lcd_backlight_off()
{
  HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
  HAL_DAC_Stop(&hdac2, DAC_CHANNEL_1);
}

void lcd_backlight_set(uint8_t brightness)
{
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_8B_R, brightness);
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_8B_R, brightness);
  HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_8B_R, brightness);

  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
  HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
}

void lcd_backlight_on()
{
  lcd_backlight_set(255);
}

void lcd_deinit(SPI_HandleTypeDef *spi)
{
  // Chip select low.
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  // 3.3v power to display *SET* to disable supply.
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
  // Disable 1.8v.
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
  // Pull reset line(?) low. (Flakey without this)
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
}

void lcd_init(SPI_HandleTypeDef *spi, LTDC_HandleTypeDef *ltdc)
{
// Wake
// Enable 3.3v
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
  HAL_Delay(1);
  // Enable 1.8V
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
  // also assert CS, not sure where to put this yet
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  HAL_Delay(7);


// HAL_SPI_Transmit(spi, (uint8_t *)"\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55", 10, 100);
  // Lets go, bootup sequence.
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_Delay(2);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);

  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(45);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x08\x80", 2, 100);
  HAL_Delay(2);
  wdog_refresh();
  
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x6E\x80", 2, 100);
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x80\x80", 2, 100);
  
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x68\x00", 2, 100);
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\xd0\x00", 2, 100);
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x1b\x00", 2, 100);
  
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\xe0\x00", 2, 100);
  
  
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x6a\x80", 2, 100);
  
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x80\x00", 2, 100);
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  // HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, (uint8_t *)"\x14\x80", 2, 100);
  HAL_Delay(2);
  // CS
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  wdog_refresh();

  HAL_LTDC_SetAddress(ltdc,(uint32_t) &framebuffer1, 0);

  memset(framebuffer1, 0, sizeof(framebuffer1));
  memset(framebuffer2, 0, sizeof(framebuffer2));
}

void HAL_LTDC_ReloadEventCallback (LTDC_HandleTypeDef *hltdc) {
  if (active_framebuffer == 0) {
    HAL_LTDC_SetAddress(hltdc, (uint32_t) framebuffer2, 0);
  } else {
    HAL_LTDC_SetAddress(hltdc, (uint32_t) framebuffer1, 0);
  }
}

void lcd_swap(void)
{
  HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
  active_framebuffer = active_framebuffer ? 0 : 1;
}

void lcd_sync(void)
{
  void *active = lcd_get_active_buffer();
  void *inactive = lcd_get_inactive_buffer();
  memcpy(inactive, active, sizeof(framebuffer1));
}

void lcd_reset_active_buffer(void)
{
  HAL_LTDC_SetAddress(&hltdc, (uint32_t) framebuffer1, 0);
  active_framebuffer = 0;
}


static uint32_t font_6x6[96] = {0x0, 0x401084, 0x14a, 0xafabea, 0xfa38be, 0x19d1173, 0x1451144, 0x44, 0x821088, 0x442104, 0x1577dd5, 0x23880, 0x220000, 0x3800, 0x400000, 0x111110, 0xe9d72e, 0xe210c4, 0x1e2224c, 0x641106, 0x84794c, 0x64184e, 0x649c44, 0x42221e, 0x649926, 0x843926, 0x20080, 0x220080, 0x820888, 0x701c0, 0x222082, 0x401106, 0xeada0e, 0x118fe2e, 0xf8be2f, 0x1e0843e, 0xf8c62f, 0x1f0bc3f, 0x10bc3f, 0x1e8e43e, 0x118fe31, 0xe2108e, 0x64a11c, 0x928ca9, 0xf08421, 0x118d771, 0x11cd671, 0xe8c62e, 0x10be2f, 0x164c62e, 0x92be2f, 0xf8383e, 0x42109f, 0xe8c631, 0x454631, 0xaad631, 0x1151151, 0x421151, 0xe1110e, 0xe1084e, 0x1041041, 0xe4210e, 0x144, 0x1f00000, 0x104, 0x164a5c0, 0x74a4e1, 0xc10980, 0xe4b908, 0xc32880, 0x423888, 0x64314c, 0x1293842, 0x421004, 0x452008, 0xa32842, 0x421084, 0x118d560, 0xa528c0, 0x452880, 0x2328c0, 0x862980, 0x211940, 0xc41180, 0x8211c0, 0xc94a40, 0x452940, 0xaad620, 0xa21140, 0x222940, 0xc22180, 0x821888, 0x420084, 0x223082, 0x1540, 0x1f8c63f };

void lcd_draw_text_6x6(int x, int y, const char* text, unsigned short color) {
  unsigned short* framebuffer = active_framebuffer ? framebuffer2 : framebuffer1;
  unsigned short* pixel = framebuffer + x + GW_LCD_WIDTH * y;
  while(*text) {
    if (*text >= 32 && *text < 128) {
      uint32_t packed = font_6x6[*text - 32];
      int mask = 1;
      for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
          if (packed & mask && pixel >= framebuffer && pixel < framebuffer + (GW_LCD_WIDTH * GW_LCD_HEIGHT)) *pixel = color;
          mask <<= 1;
          pixel++;
        }
        pixel += GW_LCD_WIDTH - 5;
      }
      pixel -= GW_LCD_WIDTH * 5 - 6;
    } else pixel += 6;
    text++;
  }
}

uint32_t font_8x8[192] = {0x0, 0x0, 0x81c1c08, 0x80008, 0x242424, 0x0, 0x247e2424, 0x24247e, 0x3c027c18, 0x183e40, 0x10264600, 0x626408, 0x6a0c120c, 0x6e1111, 0x40808, 0x0, 0x2020408, 0x80402, 0x10100804, 0x40810, 0x7f1c2200, 0x221c, 0x3e080800, 0x808, 0x0, 0x4080800, 0x7e000000, 0x0, 0x0, 0x80800, 0x10204000, 0x20408, 0x5262423c, 0x3c464a, 0x80a0c08, 0x3e0808, 0x3040423c, 0x7e420c, 0x3840423c, 0x3c4240, 0x12141810, 0x38107f, 0x403e027e, 0x3c4240, 0x3e020438, 0x3c4242, 0x1020427e, 0x80808, 0x3c42423c, 0x3c4242, 0x7c42423c, 0x1c2040, 0x80800, 0x80800, 0x80800, 0x4080800, 0x2040810, 0x100804, 0x7e0000, 0x7e00, 0x40201008, 0x81020, 0x2040423c, 0x100010, 0x4a7a423c, 0x3c027a, 0x42422418, 0x42427e, 0x3c44443e, 0x3e4444, 0x2024438, 0x384402, 0x4444241e, 0x1e2444, 0x1c14447e, 0x7e4414, 0x1c14447e, 0xe0414, 0x2024438, 0x784472, 0x7e424242, 0x424242, 0x808081c, 0x1c0808, 0x20202070, 0x1c2222, 0xc142446, 0xc62414, 0x404040e, 0x7e4404, 0x8292aac6, 0x828282, 0x62524a46, 0x424242, 0x42422418, 0x182442, 0x3c44443e, 0xe0404, 0x4242423c, 0xc03c52, 0x3c44443e, 0x4e2414, 0x3c02423c, 0x3c4240, 0x101092fe, 0x381010, 0x42424242, 0x3c4242, 0x82828282, 0x102844, 0x92828282, 0x6c9292, 0x10284482, 0x824428, 0x10284482, 0x381010, 0x102042fe, 0xfe8408, 0x202021e, 0x1e0202, 0x8040201, 0x402010, 0x1010101e, 0x1e1010, 0x41221408, 0x0, 0x0, 0xff000000, 0x100808, 0x0, 0x403c0000, 0xfc427c, 0x74040406, 0x748c8c, 0x423c0000, 0x3c4202, 0x5c404060, 0xdc6262, 0x423c0000, 0x3c027e, 0x1c084830, 0x1c0808, 0x42bc0000, 0x3e407c42, 0x4c340406, 0x464444, 0x80c0008, 0x1c0808, 0x40600040, 0x3c424240, 0x14240406, 0x64140c, 0x808080c, 0x1c0808, 0x926e0000, 0x929292, 0x463a0000, 0x424242, 0x423c0000, 0x3c4242, 0x4c360000, 0xe04344c, 0x326c0000, 0x70202c32, 0x4c360000, 0xe0444, 0x27c0000, 0x3e403c, 0x83e0808, 0x304808, 0x42420000, 0x5c6242, 0x82820000, 0x102844, 0x92820000, 0x6c9292, 0x14220000, 0x221408, 0x42420000, 0x3e407c42, 0x103e0000, 0x3e0408, 0x6080830, 0x300808, 0x80808, 0x80808, 0x6010100c, 0xc1010, 0x324c, 0x0, 0x44281000, 0xfe8282 };

void lcd_draw_text_8x8(int x, int y, const char* text, unsigned short color) {
  unsigned short* framebuffer = active_framebuffer ? framebuffer2 : framebuffer1;
  int char_x = x;
  while(*text) {
    if (*text == '\n') {
      char_x = x;
      y += 8;
    } else if (*text >= 32 && *text < 128) {
      uint32_t packed = font_8x8[(*text - 32) * 2];
      uint32_t mask = 1;
      for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 8; col++) {
          int i = char_x + col;
          int j = y + row;
          if (packed & mask && i >= 0 && i < GW_LCD_WIDTH && j >= 0 && j < GW_LCD_HEIGHT)
            framebuffer[i + j * GW_LCD_WIDTH] = color;
          mask <<= 1;
        }
      }
      packed = font_8x8[(*text - 32) * 2 + 1];
      mask = 1;
      for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 8; col++) {
          int i = char_x + col;
          int j = y + row + 4;
          if (packed & mask && i >= 0 && i < GW_LCD_WIDTH && j >= 0 && j < GW_LCD_HEIGHT)
            framebuffer[i + j * GW_LCD_WIDTH] = color;
          mask <<= 1;
        }
      }
      char_x += 8;
    }
    text++;
  }
}

