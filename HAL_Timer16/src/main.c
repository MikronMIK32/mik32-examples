#include "main.h"
#include <gpio.h>

#define PIN_TRIGGER 4 // вывод PORT_0_4
#define PIN_PWM 7 // вывод PORT_0_7

Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);


int main()
{    

    SystemClock_Config();

    HAL_RCC_ClockEnable(HAL_CLOCK_GPIO_1);
    PAD_CONFIG->PORT_1_CFG |= (1 << (2 * 9)); // Установка порта 1 в режим GPIO
	GPIO_1->DIRECTION_IN = 1 << 9; // Установка направления порта в вход


    Timer16_1_Init();

    xprintf("CFGR.B = %d\n", htimer16_1.Instance->CFGR);
    xprintf("CR.B = %d\n", htimer16_1.Instance->CR);

    HAL_Timer16_StartSingleMode(&htimer16_1);
    //HAL_Timer16_StartContinuousMode(&htimer16_1);
    //HAL_Timer16_StartPWM(&htimer16_1, 0xFFFF, 0xFFFF/2);
    //HAL_Timer16_StartOneShot(&htimer16_1, 0xFFFF, 0xFFFF/2);
    //HAL_Timer16_StartSetOnes(&htimer16_1, 0xFFFF, 0xFFFF/2);

    //HAL_Timer16_WaitTrigger(&htimer16_1);
    
    while (1)
    {    
        //HAL_Timer16_ClearTriggerFlag(&htimer16_1);
        xprintf("Counter = %d\n", HAL_Timer16_GetCounterValue(&htimer16_1));
    }
       
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_TIMER16_1_M;    
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Timer16_1_Init(void)
{
    htimer16_1.Instance = TIMER16_1;

    /* Режим работы таймера */
    htimer16_1.TimerMode = TIMER16_MODE_COUNTER;

    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_OSC32K;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_1;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактированиии от Input1 */

    /* Настройка верхнего предела счета */
    htimer16_1.Period = 0xFFFF;
    /* Настрйока режима обновления регистра ARR и CMP */
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;

    /* Настройка тригера */
    htimer16_1.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    htimer16_1.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;    /* При использовании триггера значение доложно быть отлично от software */

    /* Настройки фильтра */
    htimer16_1.Filter.ExternalClock = TIMER16_FILTER_NONE;
    htimer16_1.Filter.Trigger = TIMER16_FILTER_NONE;

    HAL_Timer16_Init(&htimer16_1);
}
