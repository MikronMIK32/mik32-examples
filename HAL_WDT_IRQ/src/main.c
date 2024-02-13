#include "mik32_hal.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_irq.h"
#include "mik32_hal_wdt.h"
#include "mik32_hal_gpio.h"

#include "uart_lib.h"
#include "xprintf.h"

WDT_HandleTypeDef hwdt;

void SystemClock_Config(void);
void WDT_Init(void);
void GPIO_Init(void);

/*
 * Пример для отладочной платы lite, у которой к выводу PORT2_7 подключен светодиод.
 *  
 * В данном примере демонстрируется работа сторожевого таймера WDT с использованием прерыванаия.
 * В начале программа инициализирует такие периферии как GPIO, UART и WDT. Затем по UART отправляется строка "Start",
 * а вывод PORT2_7 несколько раз быстро мигает светодиодом, что говорит о успешном старте программы. После этого запускается таймер WDT,
 * который отсчитывает 1000 мс. За 256 тактов таймера формируется прерывание, в обработчике которого PORT2_7 меняет свое состояние, а счетчик 
 * таймера сбрасывается.
 **/

int main()
{
    HAL_Init();

    SystemClock_Config();

    GPIO_Init();

    WDT_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    xprintf("Start\n");

    /* Индикация инициализации светодиодом на выводе PORT2_7 */
    for (volatile uint32_t i = 0; i < 5; i++)
    {
        HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
        for (volatile uint32_t j = 0; j < 100000; j++)
            ;
    }

    /* Разрешить прерывание по фронту для линии EPIC WDT */
    HAL_EPIC_MaskEdgeSet(HAL_EPIC_WDT_MASK);
    /* Разрешить глобальные прерывания */
    HAL_IRQ_EnableInterrupts();

    /* Запуск таймера WDT */
    HAL_WDT_Start(&hwdt, WDT_TIMEOUT_DEFAULT);

    while (1)
    {
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
    PCC_OscInit.LSI32KCalibrationValue = 128;
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

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_LOW);
}

void trap_handler()
{
    if (EPIC_CHECK_WDT())
    {
        HAL_WDT_Refresh(&hwdt, WDT_TIMEOUT_DEFAULT);    /* Перезагрузить значение таймера WDT */
        HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}