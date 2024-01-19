#include "mik32_hal_pcc.h"
#include "mik32_hal_spifi.h"
#include "mik32_hal_spifi_w25.h"

#include "uart_lib.h"
#include "xprintf.h"

/**
 * @file main.c
 * 
 * @brief Пример демонстрирует чтение и запись значений во внешнюю флеш память Winbond W25 по Standard/Single SPI
 */

void SystemClock_Config(void);

int main()
{
    SystemClock_Config();

    UART_Init(UART_0, 277, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    SPIFI_HandleTypeDef spifi = {
        .Instance = SPIFI_CONFIG};

    HAL_SPIFI_MspInit(&spifi);

    HAL_SPIFI_Reset(&spifi);

    W25_ManufacturerDeviceIDTypeDef id = HAL_SPIFI_W25_ReadManufacturerDeviceID(&spifi);

    xprintf("\n\nManufacturer ID: 0x%02x    DeviceID: 0x%02x\n", id.Manufacturer, id.Device);

    xprintf("SREG1 = %d\n", HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG1));
    xprintf("SREG2 = %d\n", HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG2));


    HAL_SPIFI_W25_SectorErase4K(&spifi, 0);

    uint8_t write_values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    xprintf("\nWrite values = ");
    for (int i = 0; i < sizeof(write_values); i++)
    {
        xprintf("0x%02X ", write_values[i]);
    }
    HAL_SPIFI_W25_PageProgram(&spifi, 0, sizeof(write_values), write_values);


    uint8_t read_values[10] = {0};
    HAL_SPIFI_W25_ReadData(&spifi, 0, sizeof(read_values), read_values);

    xprintf("\nRead values =  ");
    for (uint8_t i = 0; i < sizeof(read_values); i++)
    {
        xprintf("0x%02X ", read_values[i]);
    }

    while (1)
        ;
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
