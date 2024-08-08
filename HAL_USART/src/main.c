#include "mik32_hal_usart.h"

/*
 * Данный пример демонстрирует возможности приема и передачи данных
 * по интерфейсу USART.
 * 
 * При передаче на устройство (микроконтроллер) строки текста, символы
 * загружаются в буфер, пока не будет обнаружен символ '\n' возврата
 * каретки. Он заменяется на символ '\0' конца строки, а затем содержимое
 * буфера печатается функцией HAL_USART_Print.
 */


/* Value must be less than 255 */
#define BUFFER_LENGTH   50


USART_HandleTypeDef husart0;

void SystemClock_Config(void);
void USART_Init();


int main()
{
    SystemClock_Config();

    USART_Init();

    HAL_USART_Print(&husart0, "Start\n", USART_TIMEOUT_DEFAULT);

    char buf[BUFFER_LENGTH];
    uint8_t buf_pointer = 0;

    while (1)
    {
        HAL_USART_Receive(&husart0, buf+buf_pointer, USART_TIMEOUT_DEFAULT);
        if (buf[buf_pointer] == '\n')
        {
            buf[buf_pointer] = '\0';
            buf_pointer = 0;
            HAL_USART_Print(&husart0, buf, USART_TIMEOUT_DEFAULT);
            HAL_USART_Transmit(&husart0, '\n', USART_TIMEOUT_DEFAULT);
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


void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Enable;
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
    husart0.Modem.rts = Disable; //out
    husart0.Modem.cts = Disable; //in
    husart0.Modem.dtr = Disable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Disable;//out
    husart0.baudrate = 115200;
    HAL_USART_Init(&husart0);
}
