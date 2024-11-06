#include "mik32_hal_pcc.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_irq.h"
#include "mik32_hal.h"

#include "mik32_hal_usart.h"
#include "xprintf.h"

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
USART_HandleTypeDef husart0;

void SystemClock_Config(void);

static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);
static void USART_Init();

uint32_t word_src[] = {0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD};
uint32_t word_dst[] = {0, 0, 0, 0};

int main()
{
    HAL_Init();
    
    SystemClock_Config();

    USART_Init();
    
    DMA_Init();

#ifdef MIK32V0
    HAL_EPIC_MaskLevelSet(HAL_EPIC_DMA_GLB_ERR_MASK | HAL_EPIC_DMA_CHANNELS_MASK);
#endif

#ifdef MIK32V2
    HAL_EPIC_MaskLevelSet(HAL_EPIC_DMA_MASK);
#endif

    HAL_IRQ_EnableInterrupts();

    HAL_DMA_Start(&hdma_ch0, word_src, word_dst, sizeof(word_src) - 1);

    while (1)
    {
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

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;

    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = 0;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                /* write_burst_size должно быть кратно read_size */
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
#ifdef MIK32V0
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
        for (uint32_t i = 0; i < sizeof(word_src) / sizeof(*word_src); i++)
        {
            xprintf("word_dst[%d] = 0x%08x\n", i, word_dst[i]);
        }

        HAL_DMA_ClearGlobalIrq(&hdma);
        HAL_DMA_ClearErrorIrq(&hdma);
    }
#endif

#ifdef MIK32V2
    if (EPIC_CHECK_DMA())
    {
        xprintf("DMA_IRQ\n");

        HAL_DMA_ClearLocalIrq(&hdma);
        HAL_DMA_ClearGlobalIrq(&hdma);
        HAL_DMA_ClearErrorIrq(&hdma);
    }
#endif

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}


static void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Disable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode3;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    husart0.dma_tx_request = Disable;
    husart0.dma_rx_request = Disable;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable;
    husart0.Modem.cts = Disable;
    husart0.Modem.dtr = Disable;
    husart0.Modem.dcd = Disable;
    husart0.Modem.dsr = Disable;
    husart0.Modem.ri = Disable;
    husart0.Modem.ddis = Disable;
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}
