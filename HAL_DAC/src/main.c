#include "main.h"

#define PIN_LED2 7 // LED2 управляется выводом PORT_2_7

DAC_HandleTypeDef hdac0;

void SystemClock_Config(void);
static void DAC0_Init(void);

int main()
{    

    SystemClock_Config();

    DAC0_Init();

    while (1)
    {    
        for (uint16_t DAC_Value = 0; DAC_Value <= 0x0FFF; DAC_Value += 819)
        {
            HAL_DAC_SetValue(&hdac0, DAC_Value);
            #ifdef MIK32_DAC_DEBUG
            xprintf("DAC = %d\n", DAC_Value);
            #endif
            for (volatile int i = 0; i < 1000000; i++);
        }
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
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_ANALOG_REG_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void DAC0_Init(void)
{
    hdac0.Instance = ANALOG_REG;

    hdac0.Instance_dac = HAL_DAC1;
    hdac0.Init.DIV = 0;
    hdac0.Init.EXTRef = DAC_EXTREF_OFF;     /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac0.Init.EXTClb = DAC_EXTCLB_DACREF;  /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac0);
}
