#include "mik32_hal_dac.h"

#include "uart_lib.h"
#include "xprintf.h"


DAC_HandleTypeDef hdac1;

void SystemClock_Config(void);
static void DAC_Init(void);

int main()
{    

    SystemClock_Config();

    DAC_Init();

    while (1)
    {    
        for (uint16_t DAC_Value = 0; DAC_Value <= 0x0FFF; DAC_Value += 819)
        {
            HAL_DAC_SetValue(&hdac1, DAC_Value);

            for (volatile int i = 0; i < 1000000; i++);
        }
    }
       
}

void SystemClock_Config(void)
{
    PCC_OscInitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_OSC32K | PCC_OSCILLATORTYPE_OSC32M;   
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;                          
    PCC_OscInit.AHBDivider = 0;                             
    PCC_OscInit.APBMDivider = 0;                             
    PCC_OscInit.APBPDivider = 0;                             
    PCC_OscInit.HSI32MCalibrationValue = 0;                  
    PCC_OscInit.LSI32KCalibrationValue = 0;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}

static void DAC_Init(void)
{
    hdac1.Instance = ANALOG_REG;

    hdac1.Instance_dac = HAL_DAC1;
    hdac1.Init.DIV = 0;
    hdac1.Init.EXTRef = DAC_EXTREF_OFF;     /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac1.Init.EXTClb = DAC_EXTCLB_DACREF;  /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac1);
}
