#ifndef __WS2812B_BASIC_DRIVER_H__
#define __WS2812B_BASIC_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define LED_STRIP_PIN GPIO_PIN_8
#define LED_STRIP_PORT GPIOB
#define LED_GPIO_CLOCK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define BYTE_PER_PIXEL 3
#define NUM_LEDS 16
#define NUM_DATA_BYTES (NUM_LEDS*BYTE_PER_PIXEL) 

typedef struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}Color_t;

void LED_STRIP_Init(void);
void drawFrame(void);
void clearAll(void);
void getPixelColor(uint16_t px_index, Color_t * px_color);
void setPixel_GRB(const Color_t * Color, uint16_t px_index);
void setAll_GRB(const Color_t * color);
void setRange_GRB(const Color_t * color, uint16_t start_px_index, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __WS2812B_BASIC_DRIVER_H__ */