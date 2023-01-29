#include "main.h"

#include <pad_config.h>
#include <gpio.h>
#define PIN_LED2 7 // LED2 управляется выводом PORT_2_7


OTP_HandleTypeDef hscr1_timer;

void SystemClock_Config(void);
static void Scr1_Timer_Init(void);


int main()
{    

    SystemClock_Config();

    Scr1_Timer_Init();

    PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_LED2)); // Установка вывода 7 порта 2 в режим GPIO
	GPIO_2->DIRECTION_OUT = 1 << PIN_LED2; // Установка направления вывода 7 на выход

    while (1)
    {    
        GPIO_2->OUTPUT |= 1 << PIN_LED2;   //Установка значения вывода 7 порта 2 в высокий уровень
        HAL_DelayMs(&hscr1_timer, 1000);
        GPIO_2->OUTPUT &= ~(1 << PIN_LED2); //Установка значения вывода 7 порта 2 в низкий уровень   
        HAL_DelayMs(&hscr1_timer, 1000);
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M;    
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_GPIO_2_M;      
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Scr1_Timer_Init(void)
{
    hscr1_timer.Instance = SCR1_TIMER;

    hscr1_timer.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL;    /* Источник тактирования */
    hscr1_timer.Divider = 0;       /* Делтитель частоты 10-битное число. На данный момент делитель не устанавливается при инициализации */

    HAL_SCR1_Timer_Init(&hscr1_timer);
}
