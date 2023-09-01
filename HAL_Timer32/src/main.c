#include "mik32_hal_rcc.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_gpio.h"

#include "uart_lib.h"
#include "xprintf.h"

TIMER32_HandleTypeDef htimer32_1;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel0;

void SystemClock_Config(void);
static void Timer32_1_Init(void);

/*
 * В данном примере демонстрируется работа таймера32. 
 * Нулевой канал таймера используется в режиме захвата. На Port0_0 подается периодичный сигнал (ШИМ), частота которого измеряется и выводится в UART.
 *
 * */

int main()
{

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    HAL_PadConfig_PinMode(PORT1_4 | PORT1_3 | PORT1_2 | PORT1_1 | PORT1_0, PIN_MODE2); /* Timer32_2 */
    HAL_PadConfig_PinMode(PORT0_4 | PORT0_3 | PORT0_2 | PORT0_1 | PORT0_0, PIN_MODE2); /* Timer32_1 */
    Timer32_1_Init();

    HAL_Timer32_Channel_Enable(&htimer32_channel0);
    HAL_Timer32_Value_Clear(&htimer32_1);
    HAL_Timer32_Start(&htimer32_1);

    uint32_t value[2];
    uint32_t measure = 100;
    uint32_t average = 0;

    while (1)
    {
        average = 0;
        for (volatile int i = 0; i < measure; i++)
        {
            /* Ожидание активного фронта */
            HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32_1);
            HAL_TIMER32_VALUE_CLEAR(&htimer32_1);
            HAL_Timer32_Channel_WaitFlagCapture(&htimer32_channel0, TIMER32_TIMEOUT);
            value[0] = HAL_TIMER32_VALUE_GET(&htimer32_1);

            /* Захват */
            HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32_1);
            HAL_Timer32_Channel_WaitFlagCapture(&htimer32_channel0, TIMER32_TIMEOUT);
            value[1] = HAL_TIMER32_VALUE_GET(&htimer32_1);

            average += 32000000 / (value[1] - value[0]);
        }

        xprintf("%d Hz\n", average / measure);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_TIMER32_0_M;
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_TIMER32_1_M | PM_CLOCK_TIMER32_2_M;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Timer32_1_Init(void)
{
    htimer32_1.Instance = TIMER32_1;
    htimer32_1.Top = 0xFFFFFFFF;
    htimer32_1.State = TIMER32_STATE_DISABLE;
    htimer32_1.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32_1.Clock.Prescaler = 0;
    htimer32_1.InterruptMask = 0;
    htimer32_1.CountMode = TIMER32_COUNTMODE_FORWARD;
    HAL_Timer32_Init(&htimer32_1);

    htimer32_channel0.TimerInstance = htimer32_1.Instance;
    htimer32_channel0.ChannelIndex = TIMER32_CHANNEL_0;
    htimer32_channel0.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    htimer32_channel0.Mode = TIMER32_CHANNEL_MODE_CAPTURE;
    htimer32_channel0.CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    htimer32_channel0.OCR = htimer32_1.Top / 2;
    htimer32_channel0.Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(&htimer32_channel0);
}
