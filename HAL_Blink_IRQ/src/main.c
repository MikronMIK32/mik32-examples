#include "mik32_hal_rcc.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"


void SystemClock_Config(void);

// Использовать DIP плату

// Используется неподключенный вывод, для управления регистрами подтяжки
#define PIN_INT PORT2_5
#define PIN_INT_LINE1 GPIO_MUX_LINE_1_PORT2_5
#define PIN_INT_LINE2 GPIO_MUX_LINE_5_PORT2_5

#define PIN_LED1 PORT0_3
#define PIN_LED2 PORT1_3

int main()
{

    SystemClock_Config();

    HAL_PadConfig_PinMode(PIN_INT, PIN_MODE1);
    HAL_PadConfig_PinMode(PIN_LED1, PIN_MODE1);
    HAL_PadConfig_PinMode(PIN_LED2, PIN_MODE1);

    HAL_GPIO_PinDirection(PIN_INT, GPIO_PIN_INPUT);
    HAL_GPIO_PinDirection(PIN_LED1, GPIO_PIN_OUTPUT);
    HAL_GPIO_PinDirection(PIN_LED2, GPIO_PIN_OUTPUT);

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    HAL_PadConfig_PinPull(PIN_INT, PIN_PULL_DOWN);
    HAL_GPIO_PinWrite(PIN_LED1, GPIO_PIN_LOW);
    HAL_GPIO_PinWrite(PIN_LED2, GPIO_PIN_LOW);

    for (volatile int i = 0; i < 10; i++)
        ;

    HAL_GPIO_InitInterruptLine(PIN_INT_LINE1, GPIO_INT_MODE_CHANGE);
    HAL_GPIO_InitInterruptLine(PIN_INT_LINE2, GPIO_INT_MODE_RISING);

    HAL_EPIC_MaskLevelSet(HAL_EPIC_GPIO_IRQ_MASK);
    HAL_IRQ_EnableInterrupts();

    #define BLINK_LOOP_ITERS 1000000

    while (1)
    {
        HAL_PadConfig_PinPull(PIN_INT, PIN_PULL_UP);
        for (volatile int i = 0; i < BLINK_LOOP_ITERS; i++)
        ;
        HAL_PadConfig_PinPull(PIN_INT, PIN_PULL_DOWN);
        for (volatile int i = 0; i < BLINK_LOOP_ITERS; i++)
        ;
        // ledBlink(); /* Светодиод мигает */
        // ledButton(); /* Светодиод зажигается при нажатой кнопке */
    }
}

void GPIO_IRQHandler()
{
    if (HAL_GPIO_LineInterruptState(PIN_INT_LINE1))
    {
        HAL_GPIO_PinWrite(PIN_LED1, HAL_GPIO_LinePinState(PIN_INT_LINE1));
    }
    if (HAL_GPIO_LineInterruptState(PIN_INT_LINE2))
    {
        HAL_GPIO_PinToggle(PIN_LED2);
    }
    HAL_GPIO_ClearInterrupts();
}

void trap_handler()
{
    #ifdef MIK32_IRQ_DEBUG
    xprintf("\nTrap\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif

    GPIO_IT();

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);

    #ifdef MIK32_IRQ_DEBUG
    xprintf("Clear\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;
    RCC_OscInit.AHBDivider = 0;
    RCC_OscInit.APBMDivider = 0;
    RCC_OscInit.APBPDivider = 0;
    RCC_OscInit.HSI32MCalibrationValue = 0;
    RCC_OscInit.LSI32KCalibrationValue = 0;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_PM_M | PM_CLOCK_EPIC_M;
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_GPIO_0_M | PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_2_M | PM_CLOCK_GPIO_IRQ_M;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}
