#include "mik32_hal_rcc.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_gpio.h"

#include "uart_lib.h"
#include "xprintf.h"

#include <limits.h>

/* Тип платы */
#define BOARD_DIP

#ifdef BOARD_LITE
#define PIN_LED 	PORT2_7
#define PIN_BUTTON 	PORT2_6
#endif 

#ifdef BOARD_DIP
#define PIN_LED1 	PORT0_3
#define PIN_LED2 	PORT1_3
#endif 


void SystemClock_Config();
void GPIO_Config();


#define BLINK_LOOP_ITERS 1000000

void ledBlink() 
{
	#ifdef BOARD_LITE

    HAL_GPIO_PinWrite(PIN_LED, GPIO_PIN_HIGH);
	xprintf("ON \n");
	for (volatile int i = 0; i < BLINK_LOOP_ITERS; i++);

    HAL_GPIO_PinWrite(PIN_LED, GPIO_PIN_LOW);  
	xprintf("OFF \n");
	for (volatile int i = 0; i < BLINK_LOOP_ITERS; i++);

	#endif 

    #ifdef BOARD_DIP
    HAL_GPIO_PinToggle(PIN_LED1);
    HAL_GPIO_PinToggle(PIN_LED2);
    for (volatile int i = 0; i < BLINK_LOOP_ITERS; i++);
    #endif
}

void ledButton() 
{
	#ifdef BOARD_LITE
	if(HAL_GPIO_PinRead(PIN_BUTTON))
	{
        HAL_GPIO_PinWrite(PIN_LED, GPIO_PIN_HIGH);
        return;
	}
	#endif 

    ledBlink();
}


int main()
{    

    SystemClock_Config();
    GPIO_Config();

	UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

	while (1) {
		ledButton();
	}
       
}

void SystemClock_Config()
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;   
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 0;                  
    RCC_OscInit.LSI32KCalibrationValue = 0;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_PM_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_GPIO_0_M | PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_2_M; 
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

void GPIO_Config()
{
    #ifdef BOARD_LITE
    HAL_PadConfig_PinMode(PIN_LED, PIN_MODE1);
    HAL_PadConfig_PinMode(PIN_BUTTON, PIN_MODE1);
    HAL_PadConfig_PinPull(PIN_BUTTON, PIN_PULL_DOWN);

    HAL_GPIO_PinDirection(PIN_LED, GPIO_PIN_OUTPUT);
    HAL_GPIO_PinDirection(PIN_BUTTON, GPIO_PIN_INPUT);
    HAL_GPIO_PinWrite(PIN_LED, GPIO_PIN_LOW);
    #endif

    #ifdef BOARD_DIP
    HAL_PadConfig_PinMode(PIN_LED1, PIN_MODE1);
    HAL_PadConfig_PinMode(PIN_LED2, PIN_MODE1);

    HAL_GPIO_PinDirection(PIN_LED1, GPIO_PIN_OUTPUT);
    HAL_GPIO_PinDirection(PIN_LED2, GPIO_PIN_OUTPUT);
    HAL_GPIO_PinWrite(PIN_LED1, GPIO_PIN_LOW);
    HAL_GPIO_PinWrite(PIN_LED2, GPIO_PIN_LOW);
    #endif
}
