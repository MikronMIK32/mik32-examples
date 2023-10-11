#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
* В данном примере демонстрируется передача и прием данных ведомым по SPI с помощью DMA.
* Канал 0 DMA записывает байты из массива slave_output в регистр SPI TXDATA.
* Канал 1 DMA записывает байты из регистра SPI RXDATA в массив slave_input.
*
* Данный пример можно использовать вместе с ведущим из примера HAL_DMA_SPI_Master.
*/

SPI_HandleTypeDef hspi0;

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
DMA_ChannelHandleTypeDef hdma_ch1;

void SystemClock_Config(void);
static void SPI0_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_CH1_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);

int main()
{

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    SPI0_Init();

    DMA_Init();

    uint8_t slave_output[] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xC0, 0xC1};
    uint8_t slave_input[sizeof(slave_output)];
    for (uint32_t i = 0; i < sizeof(slave_input); i++)
    {
        slave_input[i] = 0;
    }

    while (1)
    {

        HAL_SPI_Enable(&hspi0);

        /* Передача и прием данных */
        HAL_DMA_Start(&hdma_ch0, slave_output, (void *)&hspi0.Instance->TXDATA, sizeof(slave_output) - 1);
        HAL_DMA_Start(&hdma_ch1, (void *)&hspi0.Instance->RXDATA, slave_input, sizeof(slave_input) - 1);

        if (HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT) != HAL_OK)
        {
            xprintf("Timeout CH0\n");
        }

        if (HAL_DMA_Wait(&hdma_ch1, DMA_TIMEOUT_DEFAULT) != HAL_OK)
        {
            xprintf("Timeout CH1\n");
        }

        HAL_SPI_Disable(&hspi0);

        /* Вывод принятый данных и обнуление массива master_input */
        for (uint32_t i = 0; i < sizeof(slave_input); i++)
        {
            xprintf("slave_input[%d] = 0x%02x\n", i, slave_input[i]);
            slave_input[i] = 0;
        }

        for (volatile int i = 0; i < 1000000; i++)
            ;
    }
}

void SystemClock_Config(void)
{
    PCC_OscInitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}

static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_SLAVE;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_OFF;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;

    if (HAL_SPI_Init(&hspi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;
    hdma_ch0.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 0;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 0;                /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
}

static void DMA_CH1_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch1.dma = hdma;
    hdma_ch1.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch1.ChannelInit.Channel = DMA_CHANNEL_1;
    hdma_ch1.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch1.ChannelInit.ReadMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch1.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch1.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch1.ChannelInit.ReadBurstSize = 0;                /* read_burst_size должно быть кратно read_size */
    hdma_ch1.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch1.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch1.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch1.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch1.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch1.ChannelInit.WriteBurstSize = 0;                /* write_burst_size должно быть кратно read_size */
    hdma_ch1.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch1.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
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
    DMA_CH1_Init(&hdma);
}
