#include "ws2812b-basic-driver.h"

__attribute__((aligned(8)))
static const uint16_t delay_loop_code[] = {
    0x1E40, // SUBS R0,R0,#1
    0xBF00, // NOP
    0xBF00, // NOP
    0xD2FB, // BCS .-3        (0x00 would be .+2, so 0xFB = -5 = .-3)
    0x4770  // BX LR
};

/* Take the address of the code, set LSB to indicate Thumb, and cast to void() function pointer */
#define delay_loop ((void(*)()) ((uintptr_t) delay_loop_code | 1))

static void LED_STRIP_GPIO_Init(void)
{
    LED_GPIO_CLOCK_ENABLE();
    GPIO_InitTypeDef gpio = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = LED_STRIP_PIN,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_HIGH};
    HAL_GPIO_Init(LED_STRIP_PORT, &gpio);
}

static uint8_t * ws2812b_px_data = NULL;

void LED_STRIP_Init(void)
{
    ws2812b_px_data = (uint8_t *)malloc(NUM_DATA_BYTES); 
    assert_param(data);
    LED_STRIP_GPIO_Init();
}

// The values were determined experimentally 
#if defined(TARGET_NUCLEO_F401RE) // 84Mhz
    #define DELAY_T1H() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"  \
			                "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"  \
			                "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"  \
			                "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"  \ 
			                "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"  \
			                "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"  \
			                "nop;nop;nop;nop;nop;nop;")
    #define DELAY_T1L() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
				            "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
				            "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;")  
    #define DELAY_T1LL() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	         "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	         "nop;nop;nop;nop;nop;") 
    #define DELAY_T0H() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
				            "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
				            "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;")
    #define DELAY_T0L() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	        "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	        "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	        "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
                            "nop;nop;nop;nop;nop;nop;")
    #define DELAY_T0LL() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	         "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	         "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;" \
			    	         "nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;")
#elif defined(TARGET_CORE2) // 168Mhz
    #define DELAY_T1H() delay_loop(24)
    #define DELAY_T1L() delay_loop(11)  
    #define DELAY_T1LL() delay_loop(10) 
    #define DELAY_T0H() delay_loop(11)
    #define DELAY_T0L() delay_loop(23)
    #define DELAY_T0LL() delay_loop(22)
#else
    #error "ws2812b-basic-driver: this target is not supported!"
#endif

void setPixel_GRB(const Color_t * color, uint16_t px_index)
{
    uint8_t * data = ws2812b_px_data + (px_index * BYTE_PER_PIXEL); 
	*data = color->g;
	*(data+1) = color->r;
	*(data+2) = color->b;
}

void setAll_GRB(const Color_t * color)
{
    for(int i=0;i<NUM_DATA_BYTES;i+=3)
    {
        ws2812b_px_data[i]=color->g;
        ws2812b_px_data[i+1]=color->r;
        ws2812b_px_data[i+2]=color->b;
    }
}

void getPixelColor(uint16_t px_index, Color_t * px_color){
    uint8_t * data = ws2812b_px_data + (px_index * BYTE_PER_PIXEL); 
    px_color->g = *data;
    px_color->r = *(data+1);
    px_color->b = *(data+2);
}

void clearAll(void)
{
    for(int i=0;i<NUM_DATA_BYTES;i+=1)
    {
        ws2812b_px_data[i]=0;
    }
}

void setRange_GRB(const Color_t * color, uint16_t start_px_index, uint16_t len)
{
    int start = start_px_index * BYTE_PER_PIXEL;
    int stop = (start_px_index + len) * BYTE_PER_PIXEL;

    for(int i=start;i<stop;i+=3)
    {
        ws2812b_px_data[i]=color->g;
        ws2812b_px_data[i+1]=color->r;
        ws2812b_px_data[i+2]=color->b;
    }
}

void drawFrame(void)
{
    uint8_t * data = ws2812b_px_data;
    const uint8_t * data_end = data + NUM_DATA_BYTES;
	uint8_t bit_mask = 0x80;
    uint8_t critical_interrupts_enabled = ((__get_PRIMASK() & 0x1) == 0);
    if(critical_interrupts_enabled)
	    __disable_irq();
    delay_loop(1);
	while(1)
	{
        if(bit_mask & *data)
        {
            LED_STRIP_PORT->BSRR = LED_STRIP_PIN;
            DELAY_T1H();    
            LED_STRIP_PORT->BSRR = (uint32_t)LED_STRIP_PIN << 16U;
            if(bit_mask == 1)
                DELAY_T1LL();
            else
                DELAY_T1L();        
        }
        else
        {
            LED_STRIP_PORT->BSRR = LED_STRIP_PIN;
            DELAY_T0H();
            LED_STRIP_PORT->BSRR = (uint32_t)LED_STRIP_PIN << 16U;
            if(bit_mask == 1)
                DELAY_T0LL();
            else
                DELAY_T0L();
        }
        
        if(!(bit_mask >>= 1))
        {
            data += 1;
            if(data >= data_end) break;
            bit_mask = 0x80;
        }
    }
    if(critical_interrupts_enabled)
	    __enable_irq();
}