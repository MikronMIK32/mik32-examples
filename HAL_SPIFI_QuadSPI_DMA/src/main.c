#include "mik32_hal_pcc.h"
#include "mik32_hal_spifi.h"
#include "mik32_hal_spifi_w25.h"
#include "mik32_hal_dma.h"

#include "uart_lib.h"
#include "xprintf.h"

/**
 * @file main.c
 *
 * @brief Пример демонстрирует чтение и запись значений во внешнюю флеш память Winbond W25 по Standard/Single SPI
 */

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
DMA_ChannelHandleTypeDef hdma_ch1;

void SystemClock_Config(void);
static void DMA_Init(void);
void read_flash(SPIFI_HandleTypeDef *spifi, uint32_t address, uint8_t dataLength, uint8_t *dataBytes);

SPIFI_HandleTypeDef spifi = {
    .Instance = SPIFI_CONFIG,
};

int main()
{
    SystemClock_Config();

    UART_Init(UART_0, 277, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    HAL_SPIFI_MspInit(&spifi);

    HAL_SPIFI_Reset(&spifi);

    DMA_Init();

    uint8_t write_values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t read_values[sizeof(write_values)] = {0};

    W25_ManufacturerDeviceIDTypeDef id = HAL_SPIFI_W25_ReadManufacturerDeviceID(&spifi);
    xprintf("\n\nManufacturer ID: 0x%02x    DeviceID: 0x%02x\n\n", id.Manufacturer, id.Device);

    xprintf("ENABLE QUAD\n");
    HAL_SPIFI_W25_QuadEnable(&spifi);
    xprintf("QUAD ENABLE = %d\n\n", (HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG2) & SPIFI_W25_SREG2_QUAD_ENABLE_M) >> SPIFI_W25_SREG2_QUAD_ENABLE_S);

    xprintf("ERASE SECTOR\n");
    HAL_SPIFI_W25_SectorErase4K(&spifi, 0);
    read_flash(&spifi, 0, sizeof(read_values), read_values);

    xprintf("Write values = ");
    for (int i = 0; i < sizeof(write_values); i++)
    {
        xprintf("0x%02X ", write_values[i]);
    }

    xprintf("\n");
    HAL_SPIFI_W25_PageProgram_Quad(&spifi, 0, sizeof(write_values), write_values);
    read_flash(&spifi, 0, sizeof(read_values), read_values);

    while (1)
        ;
}

void read_flash(SPIFI_HandleTypeDef *spifi, uint32_t address, uint8_t dataLength, uint8_t *dataBytes)
{
    HAL_SPIFI_W25_ReadData_Quad_IO(spifi, address, dataLength, dataBytes);
    xprintf("Read values =  ");
    for (uint8_t i = 0; i < dataLength; i++)
    {
        xprintf("0x%02X ", dataBytes[i]);
    }
    xprintf("\n\n");
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

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;

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
