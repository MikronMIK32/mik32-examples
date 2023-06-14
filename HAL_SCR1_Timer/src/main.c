#include "mik32_hal_rcc.h"
#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_gpio.h"

#include <pad_config.h>
#include <gpio.h>

/* Тип платы */
#define BOARD_DIP

#ifdef BOARD_DIP
#define PIN_LED PORT0_3
#endif 
#ifdef BOARD_LITE
#define PIN_LED PORT2_7
#endif 


SCR1_TIMER_HandleTypeDef hscr1_timer;

void SystemClock_Config(void);
static void Scr1_Timer_Init(void);


int main()
{    

    SystemClock_Config();

    Scr1_Timer_Init();

    HAL_PadConfig_PinMode(PIN_LED, PIN_MODE1);
    HAL_GPIO_PinDirection(PIN_LED, GPIO_PIN_OUTPUT);


    while (1)
    {
        HAL_GPIO_PinToggle(PIN_LED);
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
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_GPIO_0_M | PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_2_M;      
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Scr1_Timer_Init(void)
{
    hscr1_timer.Instance = SCR1_TIMER;

    hscr1_timer.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL;    /* Источник тактирования */
    hscr1_timer.Divider = 0;       /* Делитель частоты 10-битное число */

    HAL_SCR1_Timer_Init(&hscr1_timer);
}
