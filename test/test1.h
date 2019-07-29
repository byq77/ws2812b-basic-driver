#include <mbed.h>
#include <ws2812b-effects.h>

static const char * LED_COLORS[] =
{
    "#c90000", "#fda600", "#32ae00", "#0038ff", "#ffffff"
};

static int parseColorStr(const char * color_str, Color_t * color_ptr)
{
    uint32_t num;
    if(sscanf(color_str,"%*c%X",&num)!=1) 
        return 0;
    color_ptr->b = 0xff & num;
    color_ptr->g = 0xff & (num>>8);
    color_ptr->r = 0xff & (num>>16);
    return 1;
}

void test()
{
    DeepSleepLock lock;
    Color_t c;
    LED_STRIP_Init();
    int num_of_colors = sizeof(LED_COLORS)/sizeof(LED_COLORS[0]);
    Color_t meteor_color = {0xff, 0xff, 0xff};
    while(1)
    {
        for(int i=0;i <num_of_colors;i++)
        {
            parseColorStr(LED_COLORS[i],&c);
            FadeInOut(&c, 20, 2.0);
            RunningLights(&c, 30, 2.0);
            CyloneBounce(&c, 30, 2);
            Twinkle(&c, 15, 50);
            ColorWipe(&c,50);
            clearAll();
            ColorWipe(&c,50,true);
            RainbowCycle(20, 1);
            TheatreChase(&c, 100, 10);
            TheatreChaseRainbow(50, 1);
            meteorRain(&c, 10, 64, true, 30);
        }
    }
}