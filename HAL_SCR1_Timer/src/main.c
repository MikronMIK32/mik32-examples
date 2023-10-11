#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_pcc.h"

/*
 * Данный пример демонстрирует работу с системным таймером
 * Системный таймер используется для задержки между сменой сигнала на выводе GPIO.
 * В функции HAL_DelayMs задается время задержки в миллисекундах, по которому рассчитывается значение сравнение таймера.
 */

/* Тип платы */
#define BOARD_LITE

#ifdef BOARD_DIP
#define PIN_LED PORT0_3
#endif
#ifdef BOARD_LITE
#define PIN_LED PORT2_7
#endif

SCR1_TIMER_HandleTypeDef hscr1_timer;

void SystemClock_Config(void);
static void Scr1_Timer_Init(void);
void GPIO_Init();

int main()
{

    SystemClock_Config();

    GPIO_Init();

    Scr1_Timer_Init();

    while (1)
    {
#ifdef BOARD_LITE
        HAL_GPIO_TogglePin(GPIO_2, PIN_LED);
#endif

#ifdef BOARD_DIP
        HAL_GPIO_TogglePin(GPIO_0, PIN_LED);
#endif
        HAL_DelayMs(&hscr1_timer, 1000);
    }
}

void SystemClock_Config(void)
{
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

static void Scr1_Timer_Init(void)
{
    hscr1_timer.Instance = SCR1_TIMER;

    hscr1_timer.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL; /* Источник тактирования */
    hscr1_timer.Divider = 0;                              /* Делитель частоты 10-битное число */

    HAL_SCR1_Timer_Init(&hscr1_timer);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

#ifdef BOARD_LITE
    GPIO_InitStruct.Pin = PIN_LED;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
#endif

#ifdef BOARD_DIP
    GPIO_InitStruct.Pin = PIN_LED;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
#endif
}