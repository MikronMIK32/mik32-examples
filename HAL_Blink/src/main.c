#include "mik32_hal_rcc.h"
#include <pad_config.h>
#include <gpio.h>

#include "uart_lib.h"
#include "xprintf.h"

/* Тип платы */
#define BOARD_LITE_V0

#ifdef BOARD_LITE_V0
#define PIN_LED 	7 // Светодиод управляется выводом PORT_2_7
#define PIN_BUTTON 	6 // Кнопка управляет сигналом на выводе PORT_2_6
#endif 


void SystemClock_Config(void);

void ledBlink() 
{
	#ifdef BOARD_LITE_V0
	GPIO_2->OUTPUT |= 1 << PIN_LED;   // Установка сигнала вывода 7 порта 2 в высокий уровень
	xprintf("ON \n");
	for (volatile int i = 0; i < 100000; i++);
	GPIO_2->OUTPUT &= ~(1 << PIN_LED); // Установка сигнала вывода 7 порта  в низкий уровень   
	xprintf("OFF \n");
	for (volatile int i = 0; i < 100000; i++);
	#endif 
}

void ledButton() 
{
	#ifdef BOARD_LITE_V0
	if(GPIO_2->SET == (1 << PIN_BUTTON))
	{
		GPIO_2->OUTPUT |= 1 << PIN_LED;   // Установка сигнала вывода 7 порта 2 в высокий уровень
	}
	else
	{
		GPIO_2->OUTPUT &= ~(1 << PIN_LED); // Установка сигнала вывода 7 порта в низкий уровень
	}
	#endif 
}


int main()
{    

    SystemClock_Config();

	UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

	#ifdef BOARD_LITE_V0
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_LED)); 	// Установка вывода 7 порта 2 в режим GPIO
	GPIO_2->DIRECTION_OUT = 1 << PIN_LED; 				// Установка направления вывода 7 порта 2 на выход
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_BUTTON)); 	// Установка вывода 6 порта 2 в режим GPIO
	GPIO_2->DIRECTION_IN = 1 << PIN_BUTTON; 			// Установка направления вывода 6 порта 2 на вход
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


