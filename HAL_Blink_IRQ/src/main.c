#include "mik32_hal.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

/*
 * Данный пример демонстрирует работу с прерываниями GPIO.
 * Вывод PORT2_5 должен быть соединен с PORT2_6
 * 
 * В примере PORT_2_6 настроен на прерывание по восходящему фронту. Этот фронт создается выводом PORT2_5, 
 * который меняет свое состояние в цикле. Прерывание меняет значение переменной flag, в соответсвии с которой 
 * зажишаются или гаснут светодиоды. 
 * 
 * Плата выбирается ниже в #define
 */

/* Тип платы */
// #define BOARD_LITE
#define BOARD_DIP

void SystemClock_Config();
void GPIO_Init();

uint32_t flag = 0;

int main()
{
    HAL_Init();

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    GPIO_Init();

    /* Разрешить прерывания по уровню для линии EPIC GPIO_IRQ */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_GPIO_IRQ_MASK);
    /* Разрешить глобальные прерывания */
    HAL_IRQ_EnableInterrupts();

    while (1)
    {
        HAL_GPIO_TogglePin(GPIO_2, PORT2_5);
        for (volatile int i = 0; i < 100000; i++)
            ;

        if (flag)
        {
#ifdef BOARD_LITE
            HAL_GPIO_WritePin(GPIO_2, PORT2_7, GPIO_PIN_HIGH);
#endif

#ifdef BOARD_DIP
            HAL_GPIO_WritePin(GPIO_0, PORT0_3, GPIO_PIN_HIGH);
            HAL_GPIO_WritePin(GPIO_1, PORT1_3, GPIO_PIN_HIGH);
#endif
        }
        else
        {
#ifdef BOARD_LITE
            HAL_GPIO_WritePin(GPIO_2, PORT2_7, GPIO_PIN_LOW);
#endif

#ifdef BOARD_DIP
            HAL_GPIO_WritePin(GPIO_0, PORT0_3, GPIO_PIN_LOW);
            HAL_GPIO_WritePin(GPIO_1, PORT1_3, GPIO_PIN_LOW);
#endif
        }
    }
}

void SystemClock_Config()
{
    __HAL_PCC_WU_CLK_ENABLE();
    __HAL_PCC_PM_CLK_ENABLE();

    PCC_OscInitTypeDef PCC_OscInit = {0};
    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

#ifdef BOARD_LITE
    GPIO_InitStruct.Pin = PORT2_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
#endif

#ifdef BOARD_DIP
    GPIO_InitStruct.Pin = PORT0_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PORT1_3;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
#endif

    GPIO_InitStruct.Pin = PORT2_5;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PORT2_6;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
    HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT2_6_LINE_2, GPIO_INT_MODE_RISING);
}

void trap_handler()
{
    if (EPIC_CHECK_GPIO_IRQ())
    {
        if (HAL_GPIO_LineInterruptState(GPIO_LINE_2))
        {
            flag = !flag;
        }
        HAL_GPIO_ClearInterrupts();
    }

    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);
}