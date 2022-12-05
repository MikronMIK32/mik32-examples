#include "main.h"

ADC_HandleTypeDef hadc;

void SystemClock_Config(void);
static void ADC_Init(void);

int main()
{    

    SystemClock_Config();
    
    ADC_Init();

    uint16_t value = 0;
    while (1)
    {    
        //HAL_ADC_Enable(&hadc);
        //HAL_ADC_ResetEnable(&hadc);
        HAL_ADC_Single(&hadc);
        value = HAL_ADC_ReadValue(&hadc);
        xprintf("ADC%d: %d\n", hadc.Init.Sel, value); 
        //HAL_ADC_ResetDisable(&hadc);
        //HAL_ADC_Disable(&hadc);

        xprintf("ADC_INIT: Enable - %d \n", (hadc.Instance->ADC_CONFIG & (1 << ADC_EN_S)) >> ADC_EN_S);
        xprintf("ADC_INIT: Reset - %d \n\n", (hadc.Instance->ADC_CONFIG & (1 << ADC_RESETn_S)) >> ADC_RESETn_S);

        for (volatile int i = 0; i < 1000000; i++);
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_ANALOG_REG_M | PM_CLOCK_PAD_CONFIG_M;     
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void ADC_Init(void)
{
    hadc.Instance = ANALOG_REG;

    hadc.Init.Sel = ADC_CHANNEL2;
    hadc.Init.EXTRef = ADC_EXTREF_OFF; /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hadc.Init.EXTClb = ADC_EXTCLB_REF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_ADC_Init(&hadc);
}
