#include "xprintf.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_irq.h"

/**
 * Данный пример демонстрирует возможности приема и отправки данных по
 * интерфейсу USART по прирываниям.
 * 
 * Программа считывает отправленные по интерфейсу USART строки
 * длины не более BUFFER_LENGTH-1 и отправляет их по USART обратно.
 * 
 * Скорость порта USART - 115200 бод (может быть изменена, см. USART_Init())
 */

/* Значение должно быть меньше или равно 254 */
#define BUFFER_LENGTH   50


USART_HandleTypeDef husart0;

/* Буфер для хранения вводимых данных */
static char buf[BUFFER_LENGTH];
/* Указатель на текущий элемент буфера */
uint8_t buf_pointer;
/* Флаг готовности принятых данных */
volatile bool buf_ready = false;


void SystemClock_Config(void);
void USART_Init();


int main()
{
    SystemClock_Config();

    USART_Init();

    HAL_USART_Print(&husart0, "Start\n");

    __HAL_PCC_EPIC_CLK_ENABLE();
    HAL_EPIC_MaskLevelSet(HAL_EPIC_UART_0_MASK); 
    HAL_IRQ_EnableInterrupts();

    
    while (1)
    {
        /* Разрешить прерывания по признаку "регистр приемника не пуст" */
        HAL_USART_RXNE_EnableInterrupt(&husart0);

        buf_pointer = 0;
        /* Ожидание приема строки. Вводимая оканчивается символом '\n' */
        while(!buf_ready);

        /* Запретить прерывания по приему данных */
        HAL_USART_RXNE_DisableInterrupt(&husart0);

        buf_ready = false;
        
        /* Прибавляем к полученной строке символ возврата каретки */
        strcat(buf, "\n");

        /* Разрешить прерывания по признаку "передача данных завершена" */
        HAL_USART_TXC_EnableInterrupt(&husart0);

        /* Передача первого байта строки, последующие будут отправляться
           обработчиком прерываний */
        buf_pointer = 1;
        if (buf[0] != '\0') HAL_USART_WriteByte(&husart0, buf[0]);

        /* Задержка */
        //for (volatile uint32_t i=0; i<1000000; i++);
        HAL_DelayMs(1000);

        /* Запретить прерывания по передаче данных */
        HAL_USART_TXC_DisableInterrupt(&husart0);
    }
}


void trap_handler()
{
    if (EPIC_CHECK_UART_0())
    {
        /* Прием данных: запись в буфер */
        if (HAL_USART_RXNE_ReadFlag(&husart0))
        {
            buf[buf_pointer] = HAL_USART_ReadByte(&husart0);
            /* Если принят символ '\n', заменить его на '\0' (символ конца строки) */
            if (buf[buf_pointer] == '\n')
            {
                buf[buf_pointer] = '\0';
                buf_pointer = 0;
                buf_ready = true;
            }
            else
            {
                buf_pointer += 1;
                if (buf_pointer >= BUFFER_LENGTH) buf_pointer = 0;
                buf_ready = false;
            }
            HAL_USART_RXNE_ClearFlag(&husart0);
        }

        /* Передача данных: чтение из буфера */
        if (HAL_USART_TXC_ReadFlag(&husart0))
        {
            if (buf[buf_pointer] != '\0')
            {
                HAL_USART_WriteByte(&husart0, buf[buf_pointer]);
                buf_pointer += 1;
            }
            else buf_pointer = 0;
            HAL_USART_TXC_ClearFlag(&husart0);
        }
    }
   HAL_EPIC_Clear(0xFFFFFFFF);
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
