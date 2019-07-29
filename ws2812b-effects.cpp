#include <mbed.h>
#include <ticker_api.h>
#include "ws2812b-effects.h"

extern "C" {
  #include "stdlib.h"
}

static bool is_seed_set = false;

static void randomSeed(unsigned long seed)
{
    if(seed != 0)
    {
        is_seed_set = true;
        srand(seed);
    }
}

static long random(long a, long b)
{
    return (random() % (b-a)) + a;
}

static inline void setColorBrightness(const Color_t * in, Color_t * out, float brightness)
{
    out->r = brightness * in->r;
    out->g = brightness * in->g;
    out->b = brightness * in->b;
}

void RunningLights(const Color_t *c, int delay_ms, float time_s)
{
    Color_t new_color;
    int num_of_frames = time_s * 1000 / delay_ms;
    for (int j = 0; j < num_of_frames; j++)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            setColorBrightness(c,&new_color,(sin(i + j) * 127 + 128) / 255.0);
            setPixel_GRB(&new_color, i);
        }
        drawFrame();
        wait_ms(delay_ms);
    }
}

void FadeInOut(const Color_t *c, int delay_ms, float time_s)
{
    Color_t new_color;
    if (time_s < 1.0)
        time_s = 1.0;
    int num_of_frames = time_s * 1000/delay_ms;
    float k = num_of_frames - 1;
    for (int i = 0; i < num_of_frames; i++)
    {
        setColorBrightness(c,&new_color,i/k);
        setAll_GRB(&new_color);
        drawFrame();
        wait_ms(delay_ms);
    }
    for (int i = num_of_frames - 1; i >= 0; i--)
    {
        setColorBrightness(c,&new_color,i/k);
        setAll_GRB(&new_color);
        drawFrame();
        wait_ms(delay_ms);
    }
}

void Twinkle(const Color_t *c, int delay_ms, int count, bool clear)
{
    if(!is_seed_set)
        randomSeed(ticker_read_us(get_us_ticker_data()));
    if(clear)
        clearAll();
    for(int i=0;i<count;i++)
    {
        int index = random(0,NUM_LEDS);
        setPixel_GRB(c,index);
        drawFrame();
        wait_ms(delay_ms);
    }
    if(clear)
        clearAll();
}

void CyloneBounce(const Color_t *c, int delay_ms, int eye_size)
{
    int j = NUM_LEDS - (eye_size + 2);
    Color_t faded_color;
    setColorBrightness(c,&faded_color,0.1);
    for (int i = 0; i < j; i++)
    {
        clearAll();
        setPixel_GRB(&faded_color,i);
        setRange_GRB(c,i+1,eye_size);
        setPixel_GRB(&faded_color,i+eye_size+1);
        drawFrame();
        wait_ms(delay_ms);
    }
    wait_ms(delay_ms * 2);
    for (int i = j; i >=0; i--)
    {
        clearAll();
        setPixel_GRB(&faded_color,i);
        setRange_GRB(c,i+1,eye_size);
        setPixel_GRB(&faded_color,i+eye_size+1);
        drawFrame();
        wait_ms(delay_ms);
    }
    wait_ms(delay_ms * 2);
}

void ColorWipe(const Color_t *c, int delay_ms, bool reverse)
{
    if (reverse)
    {
        for (int i = NUM_LEDS; i > 0; i--)
        {
            setPixel_GRB(c, i);
            drawFrame();
            wait_ms(delay_ms);
        }
    }
    else
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            setPixel_GRB(c, i);
            drawFrame();
            wait_ms(delay_ms);
        }
    }
}

static void Wheel(Color_t * c, uint8_t WheelPos) {
  if(WheelPos < 85) {
   c->r=WheelPos * 3;
   c->g=255 - WheelPos * 3;
   c->b=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c->r=255 - WheelPos * 3;
   c->g=0;
   c->b=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c->r=0;
   c->g=WheelPos * 3;
   c->b=255 - WheelPos * 3;
  }
}

void RainbowCycle(int delay_ms, int cycle_num)
{
    Color_t color;
    for(int i=0; i<256 * cycle_num; i++)
    {
        for(int j=0; j < NUM_LEDS; j++)
        {
            Wheel(&color, ((j * 256 / NUM_LEDS) + i) & 0xff);
            setPixel_GRB(&color, j);
        }
        drawFrame();
        wait_ms(delay_ms);
    }
}

void TheatreChase(const Color_t * c, int delay_ms, int cycle_num)
{
    for(int i=0; i<cycle_num; i++)
    {
        for(int q=0;q<3;q++)
        {
            clearAll();
            for(int j=0; j< NUM_LEDS; j+=3)
            {
                setPixel_GRB(c,j+q);  
            }
            drawFrame();
            wait_ms(delay_ms);
        }
    }
}

void TheatreChaseRainbow(int delay_ms, int cycle_num)
{
    Color_t color;
    for(int i=0; i< 256 * cycle_num; i++)
    {
        for(int q=0;q<3;q++)
        {
            clearAll();
            for(int j=0; j< NUM_LEDS; j+=3)
            {
                Wheel(&color,(i+j) % 255);
                setPixel_GRB(&color,j+q);  
            }
            drawFrame();
            wait_ms(delay_ms);
        }
    }
}

static void setPixelHeatColor(int Pixel, uint8_t temperature)
{
    // Scale 'heat' down from 0-255 to 0-191
    uint8_t t192 = round((temperature / 255.0) * 191);
    Color_t color;
    // calculate ramp up from
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;                 // scale up to 0..252

    // figure out which third of the spectrum we're in:
    if (t192 > 0x80)
    { // hottest
        color.r = 255;
        color.g = 255;
        color.b = heatramp;
    }
    else if (t192 > 0x40)
    { // middle
        color.r = 255;
        color.g = heatramp;
        color.b = 0;
    }
    else
    { // coolest
        color.r = heatramp;
        color.g = 0;
        color.b = 0;
    }
    setPixel_GRB(&color,Pixel);
}

void Fire(int cooling_rate, int sparking_rate, int delay_ms) {
  static uint8_t heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((cooling_rate * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(0,255) < sparking_rate ) {
    int y = random(0,7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  drawFrame();
  wait_ms(delay_ms);
}

static void fadeToBlack(int led_num, uint8_t fade_level) {
    Color color;
    getPixelColor(led_num, &color);
    color.r=(color.r<=10)? 0 : (int) color.r-(color.r*fade_level/256);
    color.g=(color.g<=10)? 0 : (int) color.g-(color.g*fade_level/256);
    color.b=(color.b<=10)? 0 : (int) color.b-(color.b*fade_level/256);
    setPixel_GRB(&color, led_num);
}

void meteorRain(const Color_t * c, uint8_t meteorSize, uint8_t meteorTrailDecay, bool meteorRandomDecay, int delay_ms) {  
  clearAll();
  
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
    
    
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(0,10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel_GRB(c, i-j);
      } 
    }
   
    drawFrame();
    wait_ms(delay_ms);
  }
}