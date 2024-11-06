#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * В данном примере демонстрируется работа таймера32 с использованием прерываний.
 * Нулевой канал таймера используется в режиме захвата. На Port0_0 подается периодичный сигнал (ШИМ), частота которого измеряется и выводится в UART0.
 *
 * */

TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;

void SystemClock_Config(void);
static void Timer32_Init(void);

int main()
{
    SystemClock_Config();

    HAL_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Timer32_Init();

    HAL_Timer32_Value_Clear(&htimer32);
    if (HAL_Timer32_Compare_Start_IT(&htimer32, &htimer32_channel) != HAL_OK)
    {
        xprintf("Error\n");
    }

    /* Включать прерывания Timer16 рекомендуется после его инициализации */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_0_MASK | HAL_EPIC_TIMER32_1_MASK | HAL_EPIC_TIMER32_2_MASK);
    HAL_IRQ_EnableInterrupts();

    while (1)
    {
        xprintf("Timer32 0x%08x\n", HAL_Timer32_Value_Get(&htimer32));
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


static void Timer32_Init(void)
{
    htimer32.Instance = TIMER32_1;
    htimer32.Top = 0x10000000;
    htimer32.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32.Clock.Prescaler = 0;
    htimer32.InterruptMask = TIMER32_INT_OVERFLOW_M | TIMER32_INT_UNDERFLOW_M;
    htimer32.CountMode = TIMER32_COUNTMODE_FORWARD;
    if (HAL_Timer32_Init(&htimer32) != HAL_OK)
    {
        xprintf("Timer32_Init error\n");
    }

    htimer32_channel.TimerInstance = htimer32.Instance;
    htimer32_channel.ChannelIndex = TIMER32_CHANNEL_0;
    htimer32_channel.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    htimer32_channel.Mode = TIMER32_CHANNEL_MODE_COMPARE;
    htimer32_channel.CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    htimer32_channel.OCR = htimer32.Top / 2;
    htimer32_channel.Noise = TIMER32_CHANNEL_FILTER_OFF;
    if (HAL_Timer32_Channel_Init(&htimer32_channel) != HAL_OK)
    {
        xprintf("Timer32_Channel_Init error\n");
    }
}

void trap_handler()
{
    if (EPIC_CHECK_TIMER32_0())
    {
        uint32_t interrupt_status = HAL_Timer32_InterruptFlags_Get(&htimer32);
        uint32_t interrupt_mask = htimer32.Instance->INT_MASK;

        if ((interrupt_status & TIMER32_INT_OVERFLOW_M) & interrupt_mask)
        {
            xprintf("\nOVERFLOW_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_UNDERFLOW_M) & interrupt_mask)
        {
            xprintf("\nUNDERFLOW_IRQ\n");
            /* code */
        }

        HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32); /* Сброс всех флагов */
    }

    if (EPIC_CHECK_TIMER32_1())
    {
        uint32_t interrupt_status = HAL_Timer32_InterruptFlags_Get(&htimer32);
        uint32_t interrupt_mask = htimer32.Instance->INT_MASK;

        if ((interrupt_status & TIMER32_INT_OVERFLOW_M) & interrupt_mask)
        {
            xprintf("\nOVERFLOW_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_OC_M(htimer32_channel.ChannelIndex)) & interrupt_mask)
        {
            xprintf("\nCOMPARE_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_IC_M(htimer32_channel.ChannelIndex)) & interrupt_mask)
        {
            xprintf("\nCAPTURE_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_UNDERFLOW_M) & interrupt_mask)
        {
            xprintf("\nUNDERFLOW_IRQ\n");
            /* code */
        }

        HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32); /* Сброс всех флагов */
    }

    if (EPIC_CHECK_TIMER32_2())
    {
        uint32_t interrupt_status = HAL_Timer32_InterruptFlags_Get(&htimer32);
        uint32_t interrupt_mask = htimer32.Instance->INT_MASK;

        if ((interrupt_status & TIMER32_INT_OVERFLOW_M) & interrupt_mask)
        {
            xprintf("\nOVERFLOW_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_OC_M(htimer32_channel.ChannelIndex)) & interrupt_mask)
        {
            xprintf("\nCOMPARE_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_IC_M(htimer32_channel.ChannelIndex)) & interrupt_mask)
        {
            xprintf("\nCAPTURE_IRQ\n");
            /* code */
        }

        if ((interrupt_status & TIMER32_INT_UNDERFLOW_M) & interrupt_mask)
        {
            xprintf("\nUNDERFLOW_IRQ\n");
            /* code */
        }

        HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32); /* Сброс всех флагов */
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}