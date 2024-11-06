#include "mik32_hal_spi.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * Данный пример демонстрирует работу с SPI в режиме ведущего.
 * Ведущий передает и принимает от ведомого на выводе CS0 20 байт.
 *
 * Результат передачи выводится по UART0.
 * Данный пример можно использовать совместно с ведомым из примера HAL_SPI_Slave.
 */

SPI_HandleTypeDef hspi0;

void SystemClock_Config(void);
static void SPI0_Init(void);

int main()
{
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    SPI0_Init();

    uint8_t master_output[] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9};
    uint8_t master_input[sizeof(master_output)];
    

    while (1)
    {
        /* Начало передачи в ручном режиме управления CS */
        if (hspi0.Init.ManualCS == SPI_MANUALCS_ON)
        {
            __HAL_SPI_ENABLE(&hspi0);
            HAL_SPI_CS_Enable(&hspi0, SPI_CS_0);
        }

        /* Передача и прием данных */
        HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi0, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT);
        if (SPI_Status != HAL_OK)
        {
            xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi0.ErrorCode & HAL_SPI_ERROR_OVR, hspi0.ErrorCode & HAL_SPI_ERROR_MODF);
            HAL_SPI_ClearError(&hspi0);
        }

        /* Конец передачи в ручном режиме управления CS */
        if (hspi0.Init.ManualCS == SPI_MANUALCS_ON)
        {
            HAL_SPI_CS_Disable(&hspi0);
            __HAL_SPI_DISABLE(&hspi0);
        }

        /* Вывод принятый данных и обнуление массива master_input */
        for (uint32_t i = 0; i < sizeof(master_input); i++)
        {
            xprintf("master_input[%d] = 0x%02x\n", i, master_input[i]);
            master_input[i] = 0;
        }
        xprintf("\n");

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


static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi0.Init.ThresholdTX = 4;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
    hspi0.Init.ChipSelect = SPI_CS_0;      

    if (HAL_SPI_Init(&hspi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}
