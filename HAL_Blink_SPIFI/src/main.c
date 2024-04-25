#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_spifi.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "riscv_csr_encoding.h"
#include "csr.h"

/*
 * Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
 * В примере настраивается вывод, который подключенный к светодиоду, в режим GPIO.
 *
 * Плата выбирается ниже в #define
 */

/* Тип платы */
#define BOARD_LITE
// #define BOARD_DIP

void SystemClock_Config();
void GPIO_Init();

extern unsigned long __TEXT_START__;

__attribute__((section(".ram_text"))) void Startup_SPIFI_Config()
{
    HAL_SPIFI_MspInit_LL();
    
    SPIFI_HandleTypeDef spifi = {.Instance = SPIFI_CONFIG};

    const uint32_t CMD_READ_DATA =
        SPIFI_DIRECTION_INPUT |
        SPIFI_CONFIG_CMD_INTLEN(0) |
        SPIFI_CONFIG_CMD_FIELDFORM(SPIFI_FIELDFORM_ALL_SERIAL) |
        SPIFI_CONFIG_CMD_FRAMEFORM(SPIFI_FRAMEFORM_OPCODE_3ADDR) |
        SPIFI_CONFIG_CMD_OPCODE(0x03);

    HAL_SPIFI_MemoryMode_Init_LL(&spifi, 0x400FFFFF | SPIFI_CONFIG_CTRL_CACHE_EN_M, 0x90000000, CMD_READ_DATA);
}

__attribute__((section(".ram_text"))) void delay_loop()
{
    for (volatile int i = 0; i < 1000000; i++)
        ;
}

int main()
{
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M, 0, 0);

    write_csr(mtvec, &__TEXT_START__);
    Startup_SPIFI_Config();

    GPIO_Init();

    while (1)
    {

#ifdef BOARD_LITE
        HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
#endif

#ifdef BOARD_DIP
        HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
        HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3);
#endif
        for (volatile int i = 0; i < 1000000; i++)
            ;
        // delay_loop();
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

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

#ifdef BOARD_LITE
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
#endif

    HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);
    HAL_GPIO_TogglePin(GPIO_2, GPIO_PIN_7);

#ifdef BOARD_DIP
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
#endif
}
