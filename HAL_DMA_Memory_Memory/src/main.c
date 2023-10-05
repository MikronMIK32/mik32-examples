#include "mik32_hal.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_dma.h"

#include "uart_lib.h"
#include "xprintf.h"


DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;

void SystemClock_Config(void);

static void DMA_CH0_Init(DMA_InitTypeDef* hdma);
static void DMA_Init(void);

uint32_t word_src[] = { 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD};
uint32_t word_dst[] = { 0, 0, 0, 0};   
 

int main()
{    
    HAL_Init();
    
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    DMA_Init();

    HAL_DMA_Start(&hdma_ch0, word_src, word_dst, sizeof(word_src) - 1);

    if (HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT) != HAL_OK)
    {
        xprintf("Timeout\n");
    }

    for (uint32_t i = 0; i < sizeof(word_src)/sizeof(*word_src); i++)
    {
        xprintf("word_dst[%d] = 0x%08x\n", i, word_dst[i]);
    }

    

    while (1)
    {    
        
    }
       
}

void SystemClock_Config(void)
{
    PCC_OscInitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_OSC32K | PCC_OSCILLATORTYPE_OSC32M | PCC_OSCILLATORTYPE_HSI32M | PCC_OSCILLATORTYPE_LSI32K;   
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;                          
    PCC_OscInit.AHBDivider = 0;                             
    PCC_OscInit.APBMDivider = 0;                             
    PCC_OscInit.APBPDivider = 0;                             
    PCC_OscInit.HSI32MCalibrationValue = 0;                  
    PCC_OscInit.LSI32KCalibrationValue = 0;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}


static void DMA_CH0_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch0.dma = hdma;
    hdma_ch0.CFGWriteBuffer = 0;

    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;  
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD;       /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = 0;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD;     /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = 0; 
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE; 
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