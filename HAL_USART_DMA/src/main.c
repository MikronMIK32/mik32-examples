#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_dac.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_scr1_timer.h"

#include "xprintf.h"

/*
 * Данный пример демонстрирует возможности передачи данных по интерфейсту USART
 * с использованием DMA.
 *
 * В главном цикле запускается передача данных по DMA, затем в течение времени
 * задержки процессор и DMA конкурируют между собой за доступ к ресурсам памяти
 * и портов микроконтроллера.
 */

DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;

static void SystemClock_Config(void);
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);
static void _UART_Init();

char array[] = "\ntime: ************";

/* Переопределение функции Millis, источник - таймер SCR1 */
uint32_t HAL_Millis()
{
    return HAL_Time_SCR1TIM_Millis();
}


int main()
{
    HAL_Init();

    SystemClock_Config();

    DMA_Init();

    _UART_Init();

    HAL_Time_SCR1TIM_Init();

    while (1)
    {
        /* Получить значение времени */
        uint32_t time = HAL_Millis();

        /* Преобразовать значение времени в строку */
        //xsprintf(array, "%lu", time);
        utoa(time, array+7, 10);

        HAL_DMA_Start(&hdma_ch0, (void*)array, (void*)&UART_0->TXDATA, strlen(array)-1);

        /* Задержка */
        while (HAL_Millis() - time < 500);
    }
}

static void SystemClock_Config(void)
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
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_USART_0_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 0;                /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_USART_0_REQUEST;
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


static void _UART_Init()
{
    UART_Setting_TypeDef setting;

    setting.Instance = UART_0;
    setting.transmitting = Enable;
    setting.receiving = Disable;
    setting.frame = Frame_8bit;
    setting.parity_bit = Disable;
    setting.parity_bit_inversion = Disable;
    setting.bit_direction = LSB_First;
    setting.data_inversion = Disable;
    setting.tx_inversion = Disable;
    setting.rx_inversion = Disable;
    setting.swap = Disable;
    setting.lbm = Disable;
    setting.stop_bit = StopBit_1;
    setting.mode = Asynchronous_Mode;
    setting.xck_mode = XCK_Mode0;
    setting.last_byte_clock = Disable;
    setting.overwrite = Disable;
    setting.cts_processing = Disable;
    setting.rts_processing = Enable;
    setting.dma_tx_request = Enable;
    setting.dma_rx_request = Disable;
    setting.channel_mode = Duplex_Mode;
    setting.tx_break_mode = Disable;
    setting.Interrupt.ctsie = Disable;
    setting.Interrupt.eie = Disable;
    setting.Interrupt.idleie = Disable;
    setting.Interrupt.lbdie = Disable;
    setting.Interrupt.peie = Disable;
    setting.Interrupt.rxneie = Disable;
    setting.Interrupt.tcie = Disable;
    setting.Interrupt.txeie = Disable;
    setting.Modem.rts = Disable; //out
    setting.Modem.cts = Disable; //in
    setting.Modem.dtr = Disable; //out
    setting.Modem.dcd = Disable; //in
    setting.Modem.dsr = Disable; //in
    setting.Modem.ri = Disable;  //in
    setting.Modem.ddis = Enable;//out
    setting.baudrate = 9600;
    HAL_USART_Init(&setting);
}
