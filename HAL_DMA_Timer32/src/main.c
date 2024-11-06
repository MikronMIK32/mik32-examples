#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_dac.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 *
 * В данном примере по переполнению Timer32 в регистр входных данных для преобразования DAC1 с помощью DMA записываются значения,
 * которые  находятся в массиве word_src. Массив word_src содержит 100 значений внутри периода синуса, который равен 1 секунде.
 * y = 0.5 * sin(2 * pi * x) + 0.5
 *
 */

TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
DAC_HandleTypeDef hdac1;

void SystemClock_Config(void);
static void Timer32_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);
static void DAC_Init(void);

uint32_t word_src[] = {
    1706, 1813, 1920, 2026, 2131, 2234, 2334,
    2433, 2528, 2621, 2709, 2794, 2874, 2950,
    3021, 3087, 3147, 3201, 3250, 3293, 3329,
    3359, 3382, 3399, 3409, 3413, 3409, 3399,
    3382, 3359, 3329, 3293, 3250, 3201, 3147,
    3087, 3021, 2950, 2874, 2794, 2709, 2621,
    2528, 2433, 2334, 2234, 2131, 2026, 1920,
    1813, 1706, 1599, 1492, 1387, 1282, 1179,
    1078, 980, 884, 792, 703, 619, 538,
    462, 392, 326, 266, 211, 162, 120,
    84, 54, 30, 13, 3, 0, 3,
    13, 30, 54, 84, 120, 162, 211,
    266, 326, 392, 462, 538, 619, 703,
    792, 884, 980, 1078, 1179, 1282, 1387,
    1492, 1599};

int main()
{
    HAL_Init();

    SystemClock_Config();

    DMA_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    DAC_Init();

    Timer32_Init();

    HAL_Timer32_Channel_Enable(&htimer32_channel);
    HAL_Timer32_Value_Clear(&htimer32);
    HAL_Timer32_Start(&htimer32);

    while (1)
    {
        HAL_DMA_Start(&hdma_ch0, (void *)&word_src, (void *)&hdac1.Instance_dac->VALUE, sizeof(word_src) - 1);
        if (HAL_DMA_Wait(&hdma_ch0, 10 * DMA_TIMEOUT_DEFAULT) != HAL_OK)
        {
            xprintf("Timeout\n");
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

static void Timer32_Init(void)
{
    htimer32.Instance = TIMER32_1;
    htimer32.Top = 320000;
    htimer32.State = TIMER32_STATE_DISABLE;
    htimer32.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32.Clock.Prescaler = 0;
    htimer32.InterruptMask = 0;
    htimer32.CountMode = TIMER32_COUNTMODE_FORWARD;
    HAL_Timer32_Init(&htimer32);

    htimer32_channel.TimerInstance = htimer32.Instance;
    htimer32_channel.ChannelIndex = TIMER32_CHANNEL_0;
    htimer32_channel.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    htimer32_channel.Mode = TIMER32_CHANNEL_MODE_COMPARE;
    htimer32_channel.CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    htimer32_channel.OCR = htimer32.Top / 2;
    htimer32_channel.Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(&htimer32_channel);
}

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;

    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD; /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_TIMER32_1_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_TIMER32_1_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_ENABLE;
}

static void DMA_Init(void)
{

    /* Настройки DMA */
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        xprintf("DMA_Init Error\n");
    }

    /* Инициализация канала */
    DMA_CH0_Init(&hdma);
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
