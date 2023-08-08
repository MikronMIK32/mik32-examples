#include "mik32_hal_rcc.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_irq.h"

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
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    DMA_Init();

    HAL_EPIC_MaskLevelSet(HAL_EPIC_DMA_GLB_ERR_MASK | HAL_EPIC_DMA_CHANNELS_MASK); 
    HAL_IRQ_EnableInterrupts();

    HAL_DMA_Start(&hdma_ch0, word_src, word_dst, sizeof(word_src) - 1);

    while (1)
    {    

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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_EPIC_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M; 
    HAL_RCC_ClockConfig(&PeriphClkInit);
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
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;       /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = 0;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;     /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = 0; 
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE; 

    /* Настройка локального прерывания */
    HAL_DMA_LocalIRQEnable(&hdma_ch0, DMA_IRQ_ENABLE);
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

    /* Настройка глобального прерывания DMA */
    HAL_DMA_GlobalIRQEnable(&hdma, DMA_IRQ_ENABLE);
    /* Настройка прерывания DMA при возникновении ошибки */
    HAL_DMA_ErrorIRQEnable(&hdma, DMA_IRQ_ENABLE);

    /* Инициализация канала */
    DMA_CH0_Init(&hdma);

}


void trap_handler()
{
    #ifdef MIK32_IRQ_DEBUG
    xprintf("\nTrap\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif

    /* Локальное прерывание */
    if (EPIC_CHECK_DMA_CHANNELS())
    {
        xprintf("Local_IRQ\n");

        HAL_DMA_ClearLocalIrq(&hdma);
    }

    /* Глобальное прерывание и прерывание ошибки  */
    if (EPIC_CHECK_DMA_GLB_ERR())
    {
        xprintf("GLB_IRQ OR ERR_IRQ \n");
        for (uint32_t i = 0; i < sizeof(word_src)/sizeof(*word_src); i++)
        {
            xprintf("word_dst[%d] = 0x%08x\n", i, word_dst[i]);
        }

        HAL_DMA_ClearGlobalIrq(&hdma);
        HAL_DMA_ClearErrorIrq(&hdma);
    }

    

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);


    #ifdef MIK32_IRQ_DEBUG
    xprintf("Clear\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif
}