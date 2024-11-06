#include "mik32_hal_dac.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * В данном примере демонстрируется работа с ЦАП1
 *
 * На вывод PORT_1_12 задается напряжение, которое периодически увеличивается от 0В до 1,2В.
 *
 */

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

            HAL_DelayMs(50);
        }
    }
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

static void DAC_Init(void)
{
    hdac1.Instance = ANALOG_REG;

    hdac1.Instance_dac = HAL_DAC1;
    hdac1.Init.DIV = 0;
    hdac1.Init.EXTRef = DAC_EXTREF_OFF;    /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac1.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac1);
}
