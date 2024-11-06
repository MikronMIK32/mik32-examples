#include "mik32_hal_crc32.h"
#include "mik32_hal_usart.h"
#include "xprintf.h"

/*
* В примере демонстрируется работа с CRC32 (алгоритм CRC-32Q)
*
* Вычисляется контрольная сумма для массива байт message и для массива слов data.
* Результат выводится по UART0
*
*/


CRC_HandleTypeDef hcrc;

USART_HandleTypeDef husart0;

void SystemClock_Config(void);
static void CRC_Init(void);
void USART_Init();

int main()
{    

    SystemClock_Config();

    USART_Init();

    CRC_Init();

    uint8_t  message[] ={'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint32_t data[] = {0xABCDABCD, 0xA1B2C3D4};
    uint32_t CRCValue = 0;
    
    /* Запись по байтам */
    HAL_CRC_WriteData(&hcrc, message, sizeof(message));
    CRCValue = HAL_CRC_ReadCRC(&hcrc);
    xprintf("CRC32 = 0x%08x, ожидалось 0x3010BF7F\n", CRCValue);
    
    /* Запись по словам */
    HAL_CRC_WriteData32(&hcrc, data, sizeof(data)/sizeof(*data));
    CRCValue = HAL_CRC_ReadCRC(&hcrc);
    xprintf("CRC32 = 0x%08x, ожидалось 0x6311BC18\n", CRCValue);
    

    while (1);
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

static void CRC_Init(void)
{

    hcrc.Instance = CRC;
    
    /* Настройки вычисления CRC */                    
    /*******************************************CRC-32Q*******************************************/
    hcrc.Poly = 0x814141AB;                             
    hcrc.Init = 0x00000000;                                 
    hcrc.InputReverse = CRC_REFIN_FALSE;                 
    hcrc.OutputReverse = CRC_REFOUT_FALSE;                          
    hcrc.OutputInversion = CRC_OUTPUTINVERSION_OFF;

    HAL_CRC_Init(&hcrc);

}


void USART_Init()
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
