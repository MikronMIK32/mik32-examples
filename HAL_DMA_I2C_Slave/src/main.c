#include "mik32_hal_i2c.h"
#include "mik32_hal_dma.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * В данном примере демонстрируется работа DMA с I2C в режиме ведомого.
 * Можно использовать вместе с ведущим из примера HAL_DMA_I2C_Master.
 *
 * Ведомый читает 10 байт от ведущего по адресу, а затем записывает столько же байт.
 * Канал 0 DMA записывает из массива data в регистр I2C TXDR.
 * Канал 1 DMA считывает из регистра I2C RXDR в массив data.
 *
 * Результат выводится в UART0.
 *
 * При использовании DMA передача более 255 байт не поддерживается в HAL.
 */

I2C_HandleTypeDef hi2c0;

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0_tx;
DMA_ChannelHandleTypeDef hdma_ch1_rx;

void SystemClock_Config(void);
static void I2C0_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_CH1_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);

int main()
{

    SystemClock_Config();

    DMA_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    I2C0_Init();

    uint8_t data[10];

    HAL_StatusTypeDef error_code;
    while (1)
    {
        /* Прием данных */
        xprintf("\nSlave_Receive\n");
        HAL_I2C_Reset(&hi2c0);
        error_code = HAL_I2C_Slave_Receive_DMA(&hi2c0, data, sizeof(data), 5 * I2C_TIMEOUT_DEFAULT);
        if (error_code != HAL_OK)
        {
            xprintf("Slave_Receive error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
        }
        else
        {
            if (HAL_DMA_Wait(&hdma_ch1_rx, 5 * DMA_TIMEOUT_DEFAULT) != HAL_OK)
            {
                xprintf("Timeout CH1, BusError %d, ISR %d\n", HAL_DMA_GetBusError(&hdma_ch1_rx), hi2c0.Instance->ISR);
                HAL_DMA_ChannelDisable(&hdma_ch1_rx);
            }
            else
            {
                for (int i = 0; i < sizeof(data); i++)
                {
                    xprintf("Data_read[%d] = %d\n", i, data[i]);
                    data[i] = 0;
                }
            }
        }

        for (int i = 0; i < sizeof(data); i++)
        {
            data[i] = (uint8_t)i;
        }
        /* Отправка данных по адресу 0x36 */
        xprintf("\nSlave_Transmit\n");
        HAL_I2C_Reset(&hi2c0);
        error_code = HAL_I2C_Slave_Transmit_DMA(&hi2c0, data, sizeof(data), 5 * I2C_TIMEOUT_DEFAULT);
        if (error_code != HAL_OK)
        {
            xprintf("Slave_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
        }
        else
        {
            if (HAL_DMA_Wait(&hdma_ch0_tx, 5 * DMA_TIMEOUT_DEFAULT) != HAL_OK)
            {
                xprintf("Timeout CH0, BusError %d, ISR %d\n", HAL_DMA_GetBusError(&hdma_ch0_tx), hi2c0.Instance->ISR);
                HAL_DMA_ChannelDisable(&hdma_ch0_tx);
            }
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

static void I2C0_Init(void)
{

    /*Общие настройки*/
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_SLAVE;

    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    hi2c0.Init.OwnAddress1 = 0x36;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;
    hi2c0.Init.OwnAddress2 = 0x55;
    hi2c0.Init.GeneralCall = I2C_GENERALCALL_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;

    /* Настройка частоты */
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 15;
    hi2c0.Clock.SDADEL = 15;
    hi2c0.Clock.SCLH = 15;
    hi2c0.Clock.SCLL = 15;

    /* Каналы DMA */
    hi2c0.hdmatx = &hdma_ch0_tx;
    hi2c0.hdmarx = &hdma_ch1_rx;

    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }
}

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0_tx.dma = hdma;
    hdma_ch0_tx.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch0_tx.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0_tx.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch0_tx.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0_tx.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0_tx.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch0_tx.ChannelInit.ReadBurstSize = 0;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0_tx.ChannelInit.ReadRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch0_tx.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0_tx.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0_tx.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0_tx.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch0_tx.ChannelInit.WriteBurstSize = 0;                /* write_burst_size должно быть кратно read_size */
    hdma_ch0_tx.ChannelInit.WriteRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch0_tx.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
}

static void DMA_CH1_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch1_rx.dma = hdma;
    hdma_ch1_rx.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch1_rx.ChannelInit.Channel = DMA_CHANNEL_1;
    hdma_ch1_rx.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch1_rx.ChannelInit.ReadMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch1_rx.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch1_rx.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch1_rx.ChannelInit.ReadBurstSize = 0;                /* read_burst_size должно быть кратно read_size */
    hdma_ch1_rx.ChannelInit.ReadRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch1_rx.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch1_rx.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch1_rx.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch1_rx.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch1_rx.ChannelInit.WriteBurstSize = 0;                /* write_burst_size должно быть кратно read_size */
    hdma_ch1_rx.ChannelInit.WriteRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch1_rx.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
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
