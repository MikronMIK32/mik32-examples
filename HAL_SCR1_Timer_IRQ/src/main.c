#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_scr1_timer.h"

/*
 * Пример для платы DIP-MIK32.
 *
 * Данный пример демонстрирует работу с прерыванием системного таймера по сравнению.
 * В примере настраивается вывод PORT1_3, который подключен к светодиоду, в режим GPIO.
 * По достижению счетчика таймера значения сравнения срабатывает прерывание, по которому
 * переменная flag устанавливается в 1. В зависимости от значения flag инвертируется сигнал
 * на выводе PORT1_3.
 **/

#define DELAY_VALUE 32000000 // Значение счетчика таймера (1 секунда при частоте ядра 32МГц).

void SystemClock_Config(void);
void GPIO_Init(void);

/* Флаг, по которому инвертируется выходной сигнал на выводе PORT1_3.
 * Устанавливается в обработчике прерывания trap_handler.
 */
volatile int flag = 0;

int main()
{
    /* Настройки тактирования. */
    SystemClock_Config();

    /* Инициализация GPIO. */
    GPIO_Init();

    /* Инициализация системного таймера. */
    HAL_SCR1_Timer_Init(HAL_SCR1_TIMER_CLKSRC_INTERNAL, 0);
    __HAL_SCR1_TIMER_SET_CMP(DELAY_VALUE);
    __HAL_SCR1_TIMER_IRQ_ENABLE(); // Разрешить прерывание системного таймера.

    while (1)
    {
        if (flag)
        {
            HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3);
            flag = 0;
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
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIO_1, GPIO_InitStruct.Pin, __LOW);
}

void trap_handler()
{
    flag = 1;
    __HAL_SCR1_TIMER_SET_TIME(0); // Сброс счетчика системного таймера.

    /* Перезапись значения сравнения используется для сброса запроса прерывания по сравнению. */
    __HAL_SCR1_TIMER_SET_CMP(DELAY_VALUE);
}