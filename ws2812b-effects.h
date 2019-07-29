#ifndef __WS2812B_EFFECTS_H__
#define __WS2812B_EFFECTS_H__

#include "ws2812b-basic-driver.h"
  
/**
 * The effects are from the article at Tweaking4All.
 * Originally they use Arduino api. Here they were adjusted to use this driver api.
 * Great Thanks to authors of this awsome effects!
 * 
 * https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
void RunningLights(const Color_t * c, int delay_ms, float time_s);
void FadeInOut(const Color_t * c, int delay_ms, float time_s);
void CyloneBounce(const Color_t *c, int delay_ms, int eye_size);
void Twinkle(const Color_t *c, int delay_ms, int count, bool clear=true);
void ColorWipe(const Color_t * c, int delay_ms, bool reverse=false);
void RainbowCycle(int delay_ms, int cycle_num);
void TheatreChase(const Color_t * c, int delay_ms, int cycle_num);
void TheatreChaseRainbow(int delay_ms, int cycle_num);
void Fire(int cooling_rate, int sparking_rate, int delay_ms);
void meteorRain(const Color_t * c, uint8_t meteorSize, uint8_t meteorTrailDecay, bool meteorRandomDecay, int delay_ms);
#endif /* __WS2812B_EFFECTS_H__ */
