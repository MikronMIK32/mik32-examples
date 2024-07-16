#include "xprintf.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_irq.h"

/* Value must be less than 255 */
#define BUFFER_LENGTH   50

UART_TypeDef husart0;


void SystemClock_Config(void);
void _UART_Init();


int main()
{
    SystemClock_Config();

    _UART_Init();

    HAL_USART_Print(UART_0, "Start\n");

    char buf[BUFFER_LENGTH];
    uint8_t buf_pointer = 0;

    while (1)
    {
        buf[buf_pointer] = HAL_USART_Receive(UART_0);
        if (buf[buf_pointer] == '\n')
        {
            buf[buf_pointer] = '\0';
            buf_pointer = 0;
            HAL_USART_Print(UART_0, buf);
            HAL_USART_Transmit(UART_0, '\n');
        }
        else
        {
            if (++buf_pointer >= BUFFER_LENGTH) buf_pointer = 0;
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


void _UART_Init()
{
    UART_Setting_TypeDef setting;
    //
    setting.Instance = UART_0;
    setting.transmitting = Enable;
    setting.receiving = Enable;
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
    setting.rts_processing = Disable;
    setting.dma_tx_request = Disable;
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
    setting.baudrate = 115200;
    HAL_USART_Init(&setting);
}
