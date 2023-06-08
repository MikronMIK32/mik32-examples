#include "mik32_hal_rcc.h"
#include "mik32_hal_gpio.h"

#include "uart_lib.h"
#include "xprintf.h"

/* Тип платы */
#define BOARD_LITE_V0

#ifdef BOARD_LITE_V0
#define PIN_LED 	PORT2_7
#define PIN_BUTTON 	PORT2_6
#endif 


void SystemClock_Config(void);

void ledBlink() 
{
	#ifdef BOARD_LITE_V0

    HAL_GPIO_PinWrite(PIN_LED, PIN_HIGH);
	xprintf("ON \n");
	for (volatile int i = 0; i < 100000; i++);

    HAL_GPIO_PinWrite(PIN_LED, PIN_LOW);  
	xprintf("OFF \n");
	for (volatile int i = 0; i < 100000; i++);

	#endif 
}

void ledButton() 
{
	#ifdef BOARD_LITE_V0

	if(HAL_GPIO_PinRead(PIN_BUTTON))
	{
        HAL_GPIO_PinWrite(PIN_LED, PIN_HIGH);
	}
	else
	{
        HAL_GPIO_PinWrite(PIN_LED, PIN_LOW);  
	}

	#endif 
}


int main()
{    

    SystemClock_Config();

	UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

	#ifdef BOARD_LITE_V0

    HAL_GPIO_PinOutputInit(PIN_LED, PIN_LOW);
    HAL_GPIO_PinInputInit(PIN_BUTTON, PIN_PULL_NONE);

	#endif 

	while (1) {
		ledBlink(); /* Светодиод мигает */
		// ledButton(); /* Светодиод зажигается при нажатой кнопке */ 
	}
       
}

void SystemClock_Config(void)
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
