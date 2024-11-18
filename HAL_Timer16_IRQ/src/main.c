#include "mik32_hal.h"
#include "mik32_hal_timer16.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);
void GPIO_Init();

/*
 * Пример для платы BOARD_LITE
 * В данном примере демонстрируется работа прерываний TIMER16_1.
 * Таймер запускается и генерирует на выводе PORT0_10 ШИМ сигнал.
 * Выводы GPIO0_3 и GPIO1_3 меняют свое состояние по достижению счетчиком
 * CNT предельного значения ARR и значения сравнения CMP соответственно.
 *
 * */

int main()
{
    HAL_Init();

    SystemClock_Config();

    GPIO_Init();

    Timer16_1_Init();

    /* Включать прерывания Timer16 рекомендуется после его инициализации */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_1_MASK);
    HAL_IRQ_EnableInterrupts();

    /* Запуск таймера в продолжительном режиме с ШИМ сигналом на выводе PORT0_10 */
    HAL_Timer16_StartPWM_IT(&htimer16_1, 0xFFFF, 0xFFFF / 2);

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
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

static void Timer16_1_Init(void)
{
    htimer16_1.Instance = TIMER16_1;

    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL; /* При тактировании от Input1 не имеет значения */
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_1;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING; /* Выбирается при тактировании от Input1 */

    /* Настройка режима обновления регистра ARR и CMP */
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;

    /* Настройка триггера */
    htimer16_1.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9;
    htimer16_1.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE; /* При использовании триггера значение должно быть отлично от software */
    htimer16_1.Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;                /* Разрешить повторное срабатывание триггера */

    /* Настройки фильтра */
    htimer16_1.Filter.ExternalClock = TIMER16_FILTER_NONE;
    htimer16_1.Filter.Trigger = TIMER16_FILTER_NONE;

    /* Настройка режима энкодера */
    htimer16_1.EncoderMode = TIMER16_ENCODER_DISABLE;

    /* Выходной сигнал */
    htimer16_1.Waveform.Enable = TIMER16_WAVEFORM_GENERATION_ENABLE;
    htimer16_1.Waveform.Polarity = TIMER16_WAVEFORM_POLARITY_NONINVERTED;

    HAL_Timer16_Init(&htimer16_1);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
}

void trap_handler()
{
    if (EPIC_CHECK_TIMER16_1())
    {
        if (__HAL_TIMER16_GET_FLAG_IT(&htimer16_1, TIMER16_FLAG_ARROK))
        {
            __HAL_TIMER16_CLEAR_FLAG(&htimer16_1, TIMER16_FLAG_ARROK);
        }

        if (__HAL_TIMER16_GET_FLAG_IT(&htimer16_1, TIMER16_FLAG_CMPOK))
        {
            __HAL_TIMER16_CLEAR_FLAG(&htimer16_1, TIMER16_FLAG_CMPOK);
        }
        
        if (__HAL_TIMER16_GET_FLAG_IT(&htimer16_1, TIMER16_FLAG_ARRM))
        {
            HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3); /* Смена сигнала PORT0_3 на противоположный */
            __HAL_TIMER16_CLEAR_FLAG(&htimer16_1, TIMER16_FLAG_ARRM);
        }

        if (__HAL_TIMER16_GET_FLAG_IT(&htimer16_1, TIMER16_FLAG_CMPM))
        {
            HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3); /* Смена сигнала PORT1_3 на противоположный */
            __HAL_TIMER16_CLEAR_FLAG(&htimer16_1, TIMER16_FLAG_CMPM);
        }

        if (__HAL_TIMER16_GET_FLAG_IT(&htimer16_1, TIMER16_FLAG_EXTTRIG))
        {
            __HAL_TIMER16_CLEAR_FLAG(&htimer16_1, TIMER16_FLAG_ARRM);
        }

        /* Сброс нескольких флагов прерываний по маске */
        // __HAL_TIMER16_CLEAR_FLAG(&htimer16_1, TIMER16_FLAG_ARRM 
        //                                     | TIMER16_FLAG_CMPM 
        //                                     | TIMER16_FLAG_ARROK  
        //                                     | TIMER16_FLAG_CMPOK  
        //                                     | TIMER16_FLAG_EXTTRIG
        //                                     ); 
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}
