#include "mik32_hal_rcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_dma.h"

#include "uart_lib.h"
#include "xprintf.h"


I2C_HandleTypeDef hi2c0;

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0_tx;
DMA_ChannelHandleTypeDef hdma_ch1_rx;

void SystemClock_Config(void);
static void I2C0_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef* hdma);
static void DMA_CH1_Init(DMA_InitTypeDef* hdma);
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
        for(int i = 0; i < sizeof(data); i++)
        {
            data[i] = (uint8_t)i;
        }
        /* Отправка данных по адресу 0x36 */
        xprintf("\nMaster_Transmit\n");
        HAL_I2C_Reset(&hi2c0);
        error_code = HAL_I2C_Master_Transmit_DMA(&hi2c0, 0x36, data, sizeof(data));
        if (error_code != HAL_OK)
        {
            xprintf("Master_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
        }
        else
        {
            if (HAL_DMA_Wait(&hdma_ch0_tx, DMA_TIMEOUT_DEFAULT) != HAL_OK)
            {
                xprintf("Timeout CH0, BusError %d, ISR %d\n", HAL_DMA_GetBusError(&hdma_ch0_tx), hi2c0.Instance->ISR);
                HAL_DMA_ChannelDisable(&hdma_ch0_tx);
            }
            if (hi2c0.Init.AutoEnd == I2C_AUTOEND_DISABLE)
            {
                hi2c0.Instance->CR2 |= I2C_CR2_STOP_M;
            }
        }
        for (volatile int i = 0; i < 1000000; i++); 


        
        /* Прием данных по адресу 0x36 */
        xprintf("\nMaster_Receive\n");
        HAL_I2C_Reset(&hi2c0);
        error_code = HAL_I2C_Master_Receive_DMA(&hi2c0, 0x36, data, sizeof(data));
        if (error_code != HAL_OK)
        {
            xprintf("Master_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
        }
        else
        {
            if (HAL_DMA_Wait(&hdma_ch1_rx, DMA_TIMEOUT_DEFAULT) != HAL_OK)
            {
                xprintf("Timeout CH1, BusError %d, ISR %d\n", HAL_DMA_GetBusError(&hdma_ch1_rx), hi2c0.Instance->ISR);
            }
            else
            {
                if (hi2c0.Init.AutoEnd == I2C_AUTOEND_DISABLE)
                {
                    hi2c0.Instance->CR2 |= I2C_CR2_STOP_M;
                }
                
                for(int i = 0; i < sizeof(data); i++)
                {
                    xprintf("Data_read[%d] = %d\n", i, data[i]);
                    data[i] = 0;
                }
            }
        }
        for (volatile int i = 0; i < 1000000; i++); 

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

    /* Общие настройки */
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_ENABLE;

    /* Настройка частоты */
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
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

static void DMA_CH0_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch0_tx.dma = hdma;
    hdma_ch0_tx.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch0_tx.ChannelInit.Channel = DMA_CHANNEL_0;  
    hdma_ch0_tx.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch0_tx.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0_tx.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0_tx.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;       /* data_len должно быть кратно read_size */
    hdma_ch0_tx.ChannelInit.ReadBurstSize = 0;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch0_tx.ChannelInit.ReadRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch0_tx.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0_tx.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0_tx.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0_tx.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;     /* data_len должно быть кратно write_size */
    hdma_ch0_tx.ChannelInit.WriteBurstSize = 0;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch0_tx.ChannelInit.WriteRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch0_tx.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;  

}

static void DMA_CH1_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch1_rx.dma = hdma;
    hdma_ch1_rx.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch1_rx.ChannelInit.Channel = DMA_CHANNEL_1;  
    hdma_ch1_rx.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch1_rx.ChannelInit.ReadMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch1_rx.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch1_rx.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;       /* data_len должно быть кратно read_size */
    hdma_ch1_rx.ChannelInit.ReadBurstSize = 0;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch1_rx.ChannelInit.ReadRequest = DMA_CHANNEL_I2C_0_REQUEST;
    hdma_ch1_rx.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch1_rx.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch1_rx.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch1_rx.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;     /* data_len должно быть кратно write_size */
    hdma_ch1_rx.ChannelInit.WriteBurstSize = 0;                    /* write_burst_size должно быть кратно read_size */
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