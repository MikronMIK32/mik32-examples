#include "mik32_hal_rcc.h"
#include "mik32_hal_pad_config.h"

void SystemClock_Config(void);


int main()
{    
    SystemClock_Config();
    
    /* Задать выводам режим GPIO */
    HAL_PadConfig_PinMode(PORT1_2 | PORT1_3 | PORT1_4, PIN_MODE1);
    // HAL_PadConfig_PortMode(PORT1, PIN2 | PIN3 | PIN4, PIN_MODE1); /* Альтернативный вариант */

    /* Задать выводам подтяжку к питанию */
    HAL_PadConfig_PinPull(PORT1_2 | PORT1_3 | PORT1_4, PULL_UP);
    // HAL_PadConfig_PortPull(PORT1, PIN2 | PIN3 | PIN4, PULL_UP); /* Альтернативный вариант */

    /* Задать выводам максимальный выходной ток 2мА */
    HAL_PadConfig_PinCurrent(PORT1_2 | PORT1_3 | PORT1_4, CURRENT_2);
    // HAL_PadConfig_PortCurrent(PORT1, PIN2 | PIN3 | PIN4, CURRENT_2); /* Альтернативный вариант */

    while (1)
    {    
    
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
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT;      
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

