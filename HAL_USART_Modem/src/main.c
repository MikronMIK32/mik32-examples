#include "xprintf.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_irq.h"
#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_gpio.h"

/*
 * Данный пример  демонстрирует возможности передачи данных по
 * интерфейсу USART с использованием модемных сигналов RTS и CTS
 * 
 * Устройство Transmitter в бесконечном цикле передает байт '\n'
 * по интерфейсу USART, устройство Receiver принимает данные,
 * но прочитывает их с задержкой 1000мкс, при этом на время
 * задержки линия RTS автоматически устанавливается в лог.1.
 * Устройство Transmitter не посылает следующий байт до того
 * момента, как RTS не будет сброшен в лог.0.
 * Если при передаче данных обнаружена ошибка timeout, на плате
 * загораются светодиоды
 *
 * Сигнал RTS ("request to send, запрос на передачу", выход,
 * соединяется с CTS внешнего устройства) используется, чтобы
 * оповестить внешнее устройство о том, что данный микроконтроллер
 * готов принять байт данных. Активный уровень низкий. Сигнал RTS
 * устанавливается в "1" аппаратно во время приема стоп-бита,
 * когда модуль USART принял данные, но процессор еще не успел их
 * прочитать. После операции чтения данных из регистра RXDATA
 * сигнал RTS автоматически сбрасывается в "0".
 *
 * Сигнал CTS ("clear to send, очищен для записи", вход,
 * соединяется с RTS внешнего устройства) оповещает
 * микроконтроллер, что устройство, принимающее данные по линиям
 * USART готово принять данные. Активный уровень низкий. Флаг TXC
 * ("передача завершена") модуля USART не будет установлен до тех
 * пор, пока на линии CTS лог.1.
 * 
 * Сигнал DTR (выход, соединяется с DSR внешнего устройства)
 * сообщает внешнему устройству, что микроконтроллер готов
 * принять данные. Линия DTR управляется программно.
 * 
 * Сигнал DSR (вход, соединяется с DTR внешнего устройства)
 * используется для того, чтобы определить, готово ли внешнее
 * устройство к приему данных. Обработка состояния линии DSR
 * должна выполняться программно, при изменении состояния линии
 * устанавливается соответствующий флаг.
 * 
 */

/* Выбор режима работы устройства */
#define Transmitter
//#define Receiver

/* Выбор платы */
//#define BOARD_LITE
#define BOARD_DIP


USART_HandleTypeDef husart0;

void SystemClock_Config(void);
void USART_Init();
void GPIO_Init();


uint32_t HAL_Micros()
{
    return HAL_Time_SCR1TIM_Micros();
}
void HAL_DelayUs(uint32_t time_us)
{
    HAL_Time_SCR1TIM_DelayUs(time_us);
}


int main()
{
    SystemClock_Config();

    GPIO_Init();

    HAL_Time_SCR1TIM_Init();

    USART_Init();

    while (1)
    {
#if defined Transmitter
        bool flag = HAL_USART_Transmit(&husart0, '\n', 100000UL);
#if defined BOARD_LITE
        HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, flag == true ? 0 : 1);
#endif
#if defined BOARD_DIP
        HAL_GPIO_WritePin(GPIO_0, GPIO_PIN_3, flag == true ? 0 : 1);
        HAL_GPIO_WritePin(GPIO_1, GPIO_PIN_3, flag == true ? 0 : 1);
#endif
#endif
#if defined Receiver
        while(!HAL_USART_RXNE_ReadFlag(&husart0));
        HAL_DelayUs(1000);
        uint8_t data = HAL_USART_ReadByte(&husart0);
        HAL_USART_Transmit(&husart0, data, USART_TIMEOUT_DEFAULT);
#endif
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
#if defined Transmitter
    husart0.rts_mode = AlwaysEnable_mode;
#endif
#if defined Receiver
    husart0.rts_mode = Modem_mode;
#endif
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
    husart0.Modem.rts = Enable; //out
    husart0.Modem.cts = Enable; //in
    husart0.Modem.dtr = Enable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Enable;//out
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

#if defined BOARD_LITE
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
#endif
#if defined BOARD_DIP
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
#endif
}
