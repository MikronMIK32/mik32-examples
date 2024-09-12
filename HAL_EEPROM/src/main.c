#include "mik32_hal_pcc.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_eeprom.h"
#include "xprintf.h"

/*
 * Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
 * В примере настраивается вывод, который подключенный к светодиоду, в режим GPIO.
 *
 * Плата выбирается ниже в #define
 */


#define EEPROM_OP_TIMEOUT 100000
#define USART_TIMEOUT 1000
#define EEPROM_PAGE_WORDS 32
#define EEPROM_PAGE_COUNT 64

#define STATUS_LED_PORT GPIO_2
#define STATUS_LED_PIN GPIO_PIN_7

USART_HandleTypeDef husart0;
HAL_EEPROM_HandleTypeDef heeprom;

void SystemClock_Config();
void USART_Init();
void EEPROM_Init();
void GPIO_Init();

int main()
{
    SystemClock_Config();

    USART_Init();

    xprintf("\n==== HAL_EEPROM Example ====\n");

    EEPROM_Init();
    GPIO_Init();


    uint32_t write_data_buf[EEPROM_PAGE_WORDS] = {};
    uint32_t read_data_buf[EEPROM_PAGE_WORDS] = {};

    xprintf("\n==== 1) Write All Pages ====\n");

    int result1 = 0;

    xprintf("Erasing...\n");
    HAL_EEPROM_Erase(&heeprom, 0, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ALL, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
    {
        write_data_buf[i] = 0x55555555;
    }

    xprintf("Writing...\n");
    HAL_EEPROM_Write(&heeprom, 0, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ALL, EEPROM_OP_TIMEOUT);

    xprintf("Reading...\n");
    for (int s = 0; s < EEPROM_PAGE_COUNT; s++)
    {
        HAL_EEPROM_Read(&heeprom, s * EEPROM_PAGE_WORDS * 4, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

        for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
        {
            if (write_data_buf[i] != 0x55555555)
            {
                result1 = 1;
            }
        }
        HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
    }

    if (result1)
    {
        xprintf("Error detected!\n");
    }
    else
    {
        xprintf("OK!\n");
    }


    xprintf("\n= 2) Write Odd/Even Pages  =\n");

    int result2 = 0;

    xprintf("Erasing Odd...\n");
    HAL_EEPROM_Erase(&heeprom, 0, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ODD, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
    {
        write_data_buf[i] = 0x55555555;
    }

    xprintf("Writing Odd...\n");
    HAL_EEPROM_Write(&heeprom, 0, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ODD, EEPROM_OP_TIMEOUT);


    xprintf("Erasing Even...\n");
    HAL_EEPROM_Erase(&heeprom, EEPROM_PAGE_WORDS * 4, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_EVEN, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
    {
        write_data_buf[i] = 0xAAAAAAAA;
    }

    xprintf("Writing Even...\n");
    HAL_EEPROM_Write(&heeprom, EEPROM_PAGE_WORDS * 4, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_EVEN, EEPROM_OP_TIMEOUT);


    xprintf("Reading...\n");
    for (int s = 0; s < EEPROM_PAGE_COUNT; s += 2)
    {
        HAL_EEPROM_Read(&heeprom, s * EEPROM_PAGE_WORDS * 4, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

        for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
        {
            if (read_data_buf[i] != 0x55555555)
            {
                result2 = 1;
            }
        }

        HAL_EEPROM_Read(&heeprom, (s + 1) * EEPROM_PAGE_WORDS * 4, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

        for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
        {
            if (read_data_buf[i] != 0xAAAAAAAA)
            {
                result2 = 1;
            }
        }

        HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
    }

    if (result2)
    {
        xprintf("Error detected!\n");
    }
    else
    {
        xprintf("OK!\n");
    }


    xprintf("\n== 3) Write Single Pages  ==\n");

    int result3 = 0;

    xprintf("Erasing...\n");
    HAL_EEPROM_Erase(&heeprom, 0, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
    {
        write_data_buf[i] = i;
    }

    xprintf("Writing...\n");
    HAL_EEPROM_Write(&heeprom, 0, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);

    xprintf("Reading...\n");
    HAL_EEPROM_Read(&heeprom, 0, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++)
    {
        if (read_data_buf[i] != i)
        {
            result3 = 1;
        }
    }
    
    if (result3)
    {
        xprintf("Error detected!\n");
    }
    else
    {
        xprintf("OK!\n");
    }

    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_HIGH);

    int result = result1 | result2 | result3;

    if (result)
    {
        xprintf("\n====== Result: Error  ======\n");
    }
    else
    {
        xprintf("\n======== Result: OK ========\n");
    }

    while (1)
    {
        if (result)
        {
            HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
            HAL_DelayMs(500);
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
    husart0.receiving = Disable;
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}

void EEPROM_Init()
{
    heeprom.Instance = EEPROM_REGS;
    heeprom.Mode = HAL_EEPROM_MODE_TWO_STAGE;
    heeprom.ErrorCorrection = HAL_EEPROM_ECC_ENABLE;
    heeprom.EnableInterrupt = HAL_EEPROM_SERR_DISABLE;

    HAL_EEPROM_Init(&heeprom);
    HAL_EEPROM_CalculateTimings(&heeprom, OSC_SYSTEM_VALUE);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    GPIO_InitStruct.Pin = STATUS_LED_PIN;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(STATUS_LED_PORT, &GPIO_InitStruct);
}
