#include "mik32_hal_rcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_dma.h"

#include "uart_lib.h"
#include "xprintf.h"

I2C_HandleTypeDef hi2c0;
uint8_t flag = 0;

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0_rx;
DMA_ChannelHandleTypeDef hdma_ch1_tx;

void SystemClock_Config(void);
static void I2C0_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef* hdma);
static void DMA_CH1_Init(DMA_InitTypeDef* hdma);
static void DMA_Init(void);

/*
* При использовании DMA передача более 255 байт не поддерживается 
* При использовании DMA режим NOSTRETCH = 1 не поддерживается
*/
int main()
{
   
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    I2C0_Init();
    DMA_Init();

    // Массив с байтами для отправки / приема
    uint8_t data[I2C_DATA_BYTES];
    uint8_t data_input[I2C_DATA_BYTES];

    for(int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i+1; 
        data_input[i] = 0;
    }

    HAL_DMA_I2C_Enable(&hi2c0);

    while (1)
    {
        xprintf("slave_read\n");
        /*Ведущий отправляет - ведомый принимает*/
        HAL_DMA_Start(&hdma_ch0_rx, (void*)&hi2c0.Instance->RXDR, data_input, sizeof(data_input) - 1);
        if (hi2c0.Init.NoStretchMode == I2C_NOSTRETCH_DISABLE)
        {
            HAL_I2C_Slave_WaitADDR(&hi2c0);
            if((HAL_I2C_Slave_GetRequestedAddress(&hi2c0) != hi2c0.Init.OwnAddress2) 
                && (hi2c0.Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT))
            {
                HAL_I2C_Slave_WaitADDR(&hi2c0);
            }
        }
        if (HAL_DMA_Wait(&hdma_ch0_rx, DMA_TIMEOUT_DEFAULT) != HAL_OK)
        {
            xprintf("Timeout CH0\n");
        }

        for(int i = 0; i < sizeof(data_input); i++)
        {
            xprintf("data_input[%d] = 0x%02x\n", i, data_input[i]);
            data_input[i] = 0;
        }
        xprintf("\n");

        xprintf("slave_write\n");
        /*Ведущий принимает - ведомый отправляет*/
        HAL_DMA_Start(&hdma_ch1_tx, data, (void*)&hi2c0.Instance->TXDR, sizeof(data_input) - 1);
        if (hi2c0.Init.NoStretchMode == I2C_NOSTRETCH_DISABLE)
        {
            HAL_I2C_Slave_WaitADDR(&hi2c0);
            if((HAL_I2C_Slave_GetRequestedAddress(&hi2c0) != hi2c0.Init.OwnAddress2) 
                && (hi2c0.Init.AddressingMode == I2C_ADDRESSINGMODE_10BIT))
            {
                HAL_I2C_Slave_WaitADDR(&hi2c0);
            }
        }
        if (HAL_DMA_Wait(&hdma_ch1_tx, DMA_TIMEOUT_DEFAULT) != HAL_OK)
        {
            xprintf("Timeout CH0\n");
        }
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

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT | PM_CLOCK_DMA_M;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_I2C_0_M;     
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void I2C0_Init(void)
{

    /*Общие настройки*/
    hi2c0.Instance = I2C_0;
    hi2c0.Mode = HAL_I2C_MODE_SLAVE;
    hi2c0.ShiftAddress = SHIFT_ADDRESS_ENABLE; 
    hi2c0.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
    hi2c0.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE; // При ENABLE в режиме ведущего значение AddressingMode не влияет
    hi2c0.Init.Filter = I2C_FILTER_OFF;

    /*Настройка частоты*/
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 10;
    hi2c0.Clock.SCLH = 16;
    hi2c0.Clock.SCLL = 16;

    /*Настройки ведомого*/
    hi2c0.Init.OwnAddress1 = 0x2BB;
    hi2c0.Init.OwnAddress2 = 0x36;
    hi2c0.Init.OwnAddress2Mask = I2C_OWNADDRESS2_MASK_DISABLE;
    hi2c0.Init.SBCMode = I2C_SBC_DISABLE;
    hi2c0.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; // Не совместим с режимом SBC. I2C_NOSTRETCH_ENABLE не работает. 

    HAL_I2C_Init(&hi2c0);

}

static void DMA_CH0_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch0_rx.dma = hdma;
    hdma_ch0_rx.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch0_rx.ChannelInit.Channel = DMA_CHANNEL_0;  
    hdma_ch0_rx.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch0_rx.ChannelInit.ReadMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0_rx.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0_rx.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;       /* data_len должно быть кратно read_size */
    hdma_ch0_rx.ChannelInit.ReadBurstSize = 0;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch0_rx.ChannelInit.ReadRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch0_rx.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0_rx.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0_rx.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0_rx.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;     /* data_len должно быть кратно write_size */
    hdma_ch0_rx.ChannelInit.WriteBurstSize = 0;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch0_rx.ChannelInit.WriteRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch0_rx.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;  

}

static void DMA_CH1_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch1_tx.dma = hdma;
    hdma_ch1_tx.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch1_tx.ChannelInit.Channel = DMA_CHANNEL_1;  
    hdma_ch1_tx.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch1_tx.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch1_tx.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch1_tx.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;       /* data_len должно быть кратно read_size */
    hdma_ch1_tx.ChannelInit.ReadBurstSize = 0;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch1_tx.ChannelInit.ReadRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch1_tx.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch1_tx.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch1_tx.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch1_tx.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;     /* data_len должно быть кратно write_size */
    hdma_ch1_tx.ChannelInit.WriteBurstSize = 0;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch1_tx.ChannelInit.WriteRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch1_tx.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;  

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


