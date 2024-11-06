#include "mik32_hal_pcc.h"
#include "analog_reg.h"
#include "mik32_hal_tsens.h"

#include "uart_lib.h"
#include "xprintf.h"

TSENS_HandleTypeDef htsens;

void SystemClock_Config(void);
void TSENS_Init(void);

/*
 * Пример для отладочной платы lite v1.0 с MIK32_V2.
 *
 * Данный пример демонстрирует работу температурного сенсора. Датчик тактируется от внешнего осциллятора OSC32K.
 * В примере запускается одно одиночное измерение температуры, после чего запускается измерение в непрерывном режиме.
 * Результат выводится по UART0.
 * */

int main()
{
    uint32_t temperature;

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    /* Инициализация температурного сенсора */
    TSENS_Init();
    xprintf("Initialization of the temperature sensor completed.\n");

    /* Запустить одиночное измерение и получить значение температуры. */
    xprintf("Single mode.\n");
    temperature = HAL_TSENS_SingleStart(&htsens, 0).value;
    xprintf("%d,%02d C\n", temperature / 100, temperature % 100);

    /* Запуск непрерывного измерения температуры. */
    xprintf("Continuous mode.\n");
    HAL_TSENS_ContinuousStart(&htsens);

    while (1)
    {
        /* Получить последнее измеренное значение температуры в непрерывном режиме. */
        temperature = HAL_TSENS_GetTemperature(&htsens);
        xprintf("%d,%02d C\n", temperature / 100, temperature % 100);
        for (volatile uint32_t i = 0; i < 1000000; i++)
            ;
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

void TSENS_Init()
{
    htsens.Instance = ANALOG_REG;

    htsens.Clock = HAL_TSENS_OSC32K;
    htsens.Frequency = 3000;

    if (HAL_TSENS_Init(&htsens) != HAL_OK)
    {
        xprintf("HAL_TSENS_Init\n");
    }
}