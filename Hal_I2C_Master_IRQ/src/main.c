#include "mik32_hal_i2c.h"
#include "mik32_hal_irq.h"
#include "mik32_hal.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * В данном примере демонстрируется работа I2C в режиме ведущего с использованием прерываний.
 * Ведущий записывает по адресу 0x36 10 байт, а затем считывает.
 * Используется режим автоматического окончания.
 * Результат выводится в UART0.
 *
 * Данный пример может быть использован совместно с ведомым из примера Hal_I2C_Slave_IRQ.
 */

I2C_HandleTypeDef hi2c0;

void SystemClock_Config(void);
static void I2C0_Init(void);

int main()
{
    HAL_Init();

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    I2C0_Init();

    HAL_EPIC_MaskLevelSet(HAL_EPIC_I2C_0_MASK);
    HAL_IRQ_EnableInterrupts();

    uint8_t data[10];

    for (int i = 0; i < sizeof(data); i++)
    {
        data[i] = (uint8_t)i;
    }

    HAL_StatusTypeDef error_code;
    while (1)
    {
        for (int i = 0; i < sizeof(data); i++)
        {
            data[i] = (uint8_t)i;
        }
        /* Передача данных по адресу 0x36 */
        xprintf("\nMaster_Transmit\n");
        error_code = HAL_I2C_Master_Transmit_IT(&hi2c0, 0x36, data, sizeof(data));
        if (error_code != HAL_OK)
        {
            xprintf("Master_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
            HAL_I2C_Reset(&hi2c0);
        }

        while (hi2c0.State == HAL_I2C_STATE_BUSY)
            ;
        if (hi2c0.Init.AutoEnd == I2C_AUTOEND_DISABLE)
        {
            hi2c0.Instance->CR2 |= I2C_CR2_STOP_M;
        }
        for (volatile int i = 0; i < 1000000; i++)
            ;

        /* Прием данных по адресу 0x36 */
        xprintf("\nMaster_Receive\n");
        error_code = HAL_I2C_Master_Receive_IT(&hi2c0, 0x36, data, sizeof(data));
        if (error_code != HAL_OK)
        {
            xprintf("Master_Receive error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
            HAL_I2C_Reset(&hi2c0);
        }

        while (hi2c0.State != HAL_I2C_STATE_END)
            ;
        if (hi2c0.Init.AutoEnd == I2C_AUTOEND_DISABLE)
        {
            hi2c0.Instance->CR2 |= I2C_CR2_STOP_M;
        }
        for (int i = 0; i < sizeof(data); i++)
        {
            xprintf("Data_read[%d] = %d\n", i, data[i]);
            data[i] = 0;
        }
        for (volatile int i = 0; i < 1000000; i++)
            ;
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

static void I2C0_Init(void)
{

    /* Общие настройки */
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_ENABLE;

    /* Настройка частоты */
    hi2c0.Clock.PRESC = 15;
    hi2c0.Clock.SCLDEL = 15;
    hi2c0.Clock.SDADEL = 15;
    hi2c0.Clock.SCLH = 15;
    hi2c0.Clock.SCLL = 15;

    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }
}

void trap_handler()
{
    if (EPIC_CHECK_I2C_0())
    {
        HAL_I2C_IRQHandler(&hi2c0);
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}