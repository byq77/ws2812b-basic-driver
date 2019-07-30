#include <mbed.h>
#include <ws2812b-effects.h>

static const char * LED_COLORS[] =
{
    "#FFFFFF", "#000000"
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
    while(1)
    {
        for(int i=0;i <num_of_colors;i++)
        {
            parseColorStr(LED_COLORS[i],&c);
            setAll_GRB(&c);
            drawFrame();
            wait_ms(50);
        }
    }
}