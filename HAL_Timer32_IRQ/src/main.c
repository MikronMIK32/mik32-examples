#include "mik32_hal_rcc.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;

void SystemClock_Config(void);
static void Timer32_Init(void);

int main()
{
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    HAL_PadConfig_PinMode(PORT1_4 | PORT1_3 | PORT1_2 | PORT1_1 | PORT1_0, PIN_MODE2); /* Timer32_2 */
    HAL_PadConfig_PinMode(PORT0_4 | PORT0_3 | PORT0_2 | PORT0_1 | PORT0_0, PIN_MODE2); /* Timer32_1 */
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
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M | RCC_OSCILLATORTYPE_LSI32K | RCC_OSCILLATORTYPE_HSI32M;
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;
    RCC_OscInit.AHBDivider = 0;
    RCC_OscInit.APBMDivider = 0;
    RCC_OscInit.APBPDivider = 0;
    RCC_OscInit.HSI32MCalibrationValue = 0;
    RCC_OscInit.LSI32KCalibrationValue = 0;
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_TIMER32_0_M | PM_CLOCK_EPIC_M;
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_TIMER32_1_M | PM_CLOCK_TIMER32_2_M;
    HAL_RCC_ClockConfig(&PeriphClkInit);
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
        uint32_t interrupt_mask = htimer32.Instance->IntMask;

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
        uint32_t interrupt_mask = htimer32.Instance->IntMask;

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
        uint32_t interrupt_mask = htimer32.Instance->IntMask;

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