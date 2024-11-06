#include "mik32_hal.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_tsens.h"
#include "mik32_hal_irq.h"
#include "mik32_hal_gpio.h"

#include "uart_lib.h"
#include "xprintf.h"

TSENS_HandleTypeDef htsens;

uint32_t tsensFlag = 0; /* Флаг наличия нового значения температуры в переменной temperature. */
uint32_t hiFlag = 0;    /* Флаг превышения верхнего порогового значения. */

void SystemClock_Config(void);
void TSENS_Init(void);
void GPIO_Init(void);

/*
 * Пример для отладочной платы lite v1.0 с MIK32_V2.
 *
 * Данный пример демонстрирует работу температурного сенсора с использованием прерываний. Датчик тактируется от внешнего осциллятора OSC32K.
 * В примере устанавливается значение верхнего порога HI_THRESHOLD, соответствующее 35 ℃. Значение нижнего порога LOW_THRESHOLD остается по умолчанию -40 ℃.
 * Затем датчик начинает измерять температуру в непрерывном режиме. При этом включаются прерывания по окончанию преобразования
 * и по выходу значения датчика за пороговые значения LOW_THRESHOLD и HI_THRESHOLD. По окончанию преобразования переменной temperature присваивается новое значение температуры.
 * При превышении верхнего порога температуры зажигается светодиод на выводе PORT2_7, устанавливается флаг hiFlag и не сбрасывается до тех пор, пока температура не опустится ниже 30 ℃.
 * Результат выводится по UART0.
 * */

int main()
{
    uint32_t temperature = 0; /* Значение температуры. */

    HAL_Init();

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    GPIO_Init();

    /* Инициализация температурного сенсора. */
    TSENS_Init();
    xprintf("Initialization of the temperature sensor completed.\n");

    /* Установить верхнее пороговое значение до 35℃. */
    HAL_TSENS_SetHiThreshold(&htsens, 35);

    /* Разрешить прерывание по уровню для линии TSENS. */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TSENS_MASK);

    /* Разрешить глобальные прерывания. */
    HAL_IRQ_EnableInterrupts();

    /* Запустить измерение температуры в непрерывном режиме. */
    HAL_TSENS_ContinuousStart_IT(&htsens);

    while (1)
    {
        if (tsensFlag) /* Если получено новое значение температуры. */
        {
            temperature = TSENS_VALUE_TO_CELSIUS(__HAL_TSENS_READ_MEASUREMENT(&htsens));
            xprintf("%d,%02d C\n", temperature / 100, temperature % 100);
            tsensFlag = 0;
            if (temperature < 3000) /* Если температура опустилась ниже 30℃ включить прерывание по превышению верхнего порогового значения. */
            {
                HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_LOW);
                __HAL_TSENS_IRQ_CLEAR_HIIRQ(&htsens); /* Очистить флаг прерывания HI_IRQ. */
                __HAL_TSENS_IRQ_ENABLE_HIIRQ(&htsens);
            }
        }

        if (hiFlag) /* Если было превышено верхнее пороговое значение HI_THRESHOLD. */
        {
            xprintf("HI_THRESHOLD\n");
            HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_HIGH);
            hiFlag = 0;
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

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_LOW);
}

void trap_handler()
{
    if (EPIC_CHECK_TSENS())
    {
        if (__HAL_TSENS_IRQ_GET_EOC_MASK(&htsens))
        {
            tsensFlag = 1;
            __HAL_TSENS_IRQ_CLEAR_EOC(&htsens); /* Очистить флаг прерывания EOC_IRQ. */
        }
        if (__HAL_TSENS_IRQ_GET_HIIRQ_MASK(&htsens))
        {
            __HAL_TSENS_IRQ_DISABLE_HIIRQ(&htsens);
            __HAL_TSENS_IRQ_CLEAR_HIIRQ(&htsens); /* Очистить флаг прерывания HI_IRQ. */
            hiFlag = 1;
        }

        __HAL_TSENS_IRQ_CLEAR_LOWIRQ(&htsens); /* Очистить флаг прерывания LOW_IRQ. */
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}