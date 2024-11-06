#include "mik32_hal_pcc.h"
#include "mik32_hal_wdt.h"
#include "uart_lib.h"
#include "xprintf.h"


WDT_HandleTypeDef hwdt;

void SystemClock_Config(void);
void WDT_Init(void);

/*
 * В данном примере демонстрируется работа сторожевого таймера WDT.
 * В начале программа инициализирует такие периферии как UART и WDT. Затем по UART отправляется строка "Start", что говорит о успешном старте программы. 
 * После этого запускается таймер WDT, который отсчитывает 1000 мс. Затем значение счетчика таймера сбрасывает функцией HAL_WDT_Refresh. После этого
 * программа заходит в бесконечный цикл, в котором таймер досчитывает до конца и формирует сигнал сброса.
 **/

int main()
{
    SystemClock_Config();

    WDT_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    xprintf("Start\n");

    /* Запуск таймера WDT */
    HAL_WDT_Start(&hwdt, WDT_TIMEOUT_DEFAULT);

    /* Сбросить значение WDT */
    HAL_WDT_Refresh(&hwdt, WDT_TIMEOUT_DEFAULT);
    xprintf("Refresh\n\n");

    while (1)
    {
        /* Бесконечный цикл для сброса контроллера по WDT */
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

void WDT_Init()
{
    hwdt.Instance = WDT;

    hwdt.Init.Clock = HAL_WDT_OSC32K;
    hwdt.Init.ReloadMs = 1000;
    if (HAL_WDT_Init(&hwdt, WDT_TIMEOUT_DEFAULT) != HAL_OK)
    {
        xprintf("hAL_WDT_Init\n");
    }
}