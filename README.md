# ws2812b basic driver
Very simple ws2812b leds' driver for stm32f4xxxx mbed's targets. It using GPIO periphery and NOPs for delays. 

Supported targets:
* `NUCLEO_F401RE`

## Configuration
Edit file `ws2812b-basic-driver.h` to change gpio pin and number of leds.