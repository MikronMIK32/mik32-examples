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

    HAL_USART_Print(UART_0, "Start\n");

    /* Включать прерывания Timer16 рекомендуется после его инициализации */
    __HAL_PCC_EPIC_CLK_ENABLE();
    HAL_EPIC_MaskLevelSet(HAL_EPIC_UART_0_MASK); 
    HAL_IRQ_EnableInterrupts();

    
    while (1)
    {
        /* Разрешить прерывания по признаку "регистр приемника не пуст" */
        HAL_USART_RXNE_EnableInterrupt(UART_0);

        buf_pointer = 0;
        /* Ожидание приема строки. Вводимая оканчивается символом '\n' */
        while(!buf_ready);

        /* Запретить прерывания по приему данных */
        HAL_USART_RXNE_DisableInterrupt(UART_0);

        buf_ready = false;
        
        /* Прибавляем к полученной строке символ возврата каретки */
        strcat(buf, "\n");

        /* Разрешить прерывания по признаку "передача данных завершена" */
        HAL_USART_TXC_EnableInterrupt(UART_0);

        /* Передача первого байта строки, последующие будут отправляться
           обработчиком прерываний */
        buf_pointer = 1;
        if (buf[0] != '\0') HAL_USART_WriteByte(UART_0, buf[0]);

        /* Задержка */
        //for (volatile uint32_t i=0; i<1000000; i++);
        HAL_DelayMs(1000);

        /* Запретить прерывания по передаче данных */
        HAL_USART_TXC_DisableInterrupt(UART_0);
    }
}


void trap_handler()
{
    if (EPIC_CHECK_UART_0())
    {
        /* Прием данных: запись в буфер */
        if (HAL_USART_RXNE_ReadFlag(UART_0))
        {
            buf[buf_pointer] = HAL_USART_ReadByte(UART_0);
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
            HAL_USART_RXNE_ClearFlag(UART_0);
        }

        /* Передача данных: чтение из буфера */
        if (HAL_USART_TXC_ReadFlag(UART_0))
        {
            if (buf[buf_pointer] != '\0')
            {
                HAL_USART_WriteByte(UART_0, buf[buf_pointer]);
                buf_pointer += 1;
            }
            else buf_pointer = 0;
            HAL_USART_TXC_ClearFlag(UART_0);
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
    UART_Setting_TypeDef setting;
    //
    setting.Instance = UART_0;
    /* Разрешить передачу данных */
    setting.transmitting = Enable;
    /* Разрешить прием данных */
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
    /* Управление прерываниями RXNE и TXC идет в цикле */
    setting.Interrupt.rxneie = Disable;
    setting.Interrupt.tcie = Disable;
    setting.Interrupt.txeie = Disable;
    setting.Modem.rts = Disable;
    setting.Modem.cts = Disable;
    setting.Modem.dtr = Disable;
    setting.Modem.dcd = Disable;
    setting.Modem.dsr = Disable;
    setting.Modem.ri = Disable;
    setting.Modem.ddis = Disable;
    setting.baudrate = 115200;
    HAL_USART_Init(&setting);
}
