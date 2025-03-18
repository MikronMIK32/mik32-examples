#include "mik32_hal_i2c.h"
#include "mik32_hal_usart.h"
#include "xprintf.h"

/*
*Программа выполняет мониторинг шины I2C, выводя в порт адреса найденных слейвов
*/

#define BAUDRATE        115200UL

//#define USE_I2C0
#define USE_I2C1

#define USE_USART0
//#define USE_USART1


I2C_HandleTypeDef hi2c;
USART_HandleTypeDef husart;

static void SystemClock_Config(void);
static void I2C_Init(void);
static void USART_Init(void);

int main()
{    
    SystemClock_Config();

    USART_Init();
    I2C_Init();

    HAL_StatusTypeDef res;

    while (1)
    {
        xprintf("Searching for devices...\n");
        for (uint8_t i=0; i<0x80; i++)
        {
            uint8_t dummy;
            res = HAL_I2C_Master_Receive(&hi2c, i, &dummy, 1, 10000);
            if (res == HAL_OK)
            {
                xprintf("I2C device found at address 0x%02X\n", i);
            }
        }
        HAL_DelayMs(2000);
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
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}


static void I2C_Init(void)
{
    /* Общие настройки */
#ifdef USE_I2C0
    hi2c.Instance = I2C_0;
#endif
#ifdef USE_I2C1
    hi2c.Instance = I2C_1;
#endif
    hi2c.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c.Init.AutoEnd = I2C_AUTOEND_ENABLE;
    /* Настройка частоты */
    hi2c.Clock.PRESC = 1;
    hi2c.Clock.SCLDEL = 15;
    hi2c.Clock.SDADEL = 15;
    hi2c.Clock.SCLH = 75;
    hi2c.Clock.SCLL = 75;
    if (HAL_I2C_Init(&hi2c) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }
}


static void USART_Init()
{
#ifdef USE_USART0
    husart.Instance = UART_0;
#endif
#ifdef USE_USART1
    husart.Instance = UART_1;
#endif
    husart.transmitting = Enable;
    husart.receiving = Disable;
    husart.frame = Frame_8bit;
    husart.parity_bit = Disable;
    husart.parity_bit_inversion = Disable;
    husart.bit_direction = LSB_First;
    husart.data_inversion = Disable;
    husart.tx_inversion = Disable;
    husart.rx_inversion = Disable;
    husart.swap = Disable;
    husart.lbm = Disable;
    husart.stop_bit = StopBit_1;
    husart.mode = Asynchronous_Mode;
    husart.xck_mode = XCK_Mode3;
    husart.last_byte_clock = Disable;
    husart.overwrite = Disable;
    husart.rts_mode = AlwaysEnable_mode;
    husart.dma_tx_request = Disable;
    husart.dma_rx_request = Disable;
    husart.channel_mode = Duplex_Mode;
    husart.tx_break_mode = Disable;
    husart.Interrupt.ctsie = Disable;
    husart.Interrupt.eie = Disable;
    husart.Interrupt.idleie = Disable;
    husart.Interrupt.lbdie = Disable;
    husart.Interrupt.peie = Disable;
    husart.Interrupt.rxneie = Disable;
    husart.Interrupt.tcie = Disable;
    husart.Interrupt.txeie = Disable;
    husart.Modem.rts = Disable; //out
    husart.Modem.cts = Disable; //in
    husart.Modem.dtr = Disable; //out
    husart.Modem.dcd = Disable; //in
    husart.Modem.dsr = Disable; //in
    husart.Modem.ri = Disable;  //in
    husart.Modem.ddis = Disable;//out
    husart.baudrate = BAUDRATE;
    HAL_USART_Init(&husart);
}
