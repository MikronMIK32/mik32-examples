#include "mik32_hal_rcc.h"
#include "mik32_hal_pad_config.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"


void SystemClock_Config(void);

/*
 * Пример для DIP платы.
 * Использование перываний GPIO на примере мигания светодиодами
 * 
 * Вывод PORT2_5 меняет режим подтяжки и создает перрывание, по которому меняестя 
 * логический уровень на выводах PORT0_3 и PORT1_3, к которым подключены светодиоды.
 * 
 */


// Используется неподключенный вывод, для управления регистрами подтяжки
#define PIN_INT PORT2_5
#define PIN_INT_LINE1 GPIO_MUX_LINE_1_PORT2_5
#define PIN_INT_LINE2 GPIO_MUX_LINE_5_PORT2_5

#define PIN_LED1 PORT0_3
#define PIN_LED2 PORT1_3

int main()
{

    SystemClock_Config();

    /* Настройка режимов выводов */
    HAL_PadConfig_PinMode(PIN_INT, PIN_MODE1);
    HAL_PadConfig_PinMode(PIN_LED1, PIN_MODE1);
    HAL_PadConfig_PinMode(PIN_LED2, PIN_MODE1);

    /* Настройка направления вывода */
    HAL_GPIO_PinDirection(PIN_INT, GPIO_PIN_INPUT);
    HAL_GPIO_PinDirection(PIN_LED1, GPIO_PIN_OUTPUT);
    HAL_GPIO_PinDirection(PIN_LED2, GPIO_PIN_OUTPUT);

    /* Подтяжка вывода PIN_INT к земле */
    HAL_PadConfig_PinPull(PIN_INT, PIN_PULL_DOWN);

    /* Задать логический ноль на выводах PIN_LED1 и PIN_LED2 */
    HAL_GPIO_PinWrite(PIN_LED1, GPIO_PIN_LOW);
    HAL_GPIO_PinWrite(PIN_LED2, GPIO_PIN_LOW);

    for (volatile int i = 0; i < 10; i++)
        ;

    /* Настройка линий прерываний и их режимов работы  */
    HAL_GPIO_InitInterruptLine(PIN_INT_LINE1, GPIO_INT_MODE_CHANGE); /* На линии 1 прерывание по смене уровня на выводе PORT2_5 */
    HAL_GPIO_InitInterruptLine(PIN_INT_LINE2, GPIO_INT_MODE_RISING); /* На линии 2 прерывание по восходящему фронту на выводе PORT2_5 */

    /* Разрешить прерывания по уровню для линии EPIC GPIO_IRQ */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_GPIO_IRQ_MASK);
    /* Разрешить глобальные прерывания */
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
    }
}

void trap_handler()
{
    #ifdef MIK32_IRQ_DEBUG
    xprintf("\nTrap\n");
    xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
    xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif

    if (EPIC_CHECK_GPIO_IRQ())
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
    RCC_OscInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    RCC_OscInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT;
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_PM_M | PM_CLOCK_EPIC_M;
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_GPIO_0_M | PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_2_M | PM_CLOCK_GPIO_IRQ_M;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}
