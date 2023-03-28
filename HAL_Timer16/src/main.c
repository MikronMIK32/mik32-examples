#include "mik32_hal_rcc.h"
#include "mik32_hal_timer16.h"

#include <gpio.h>

#include "uart_lib.h"
#include "xprintf.h"


Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);


int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    /************************Включить GPIO1.9 для триггера Timer16_1************************/
    // HAL_RCC_ClockEnable(HAL_CLOCK_GPIO_1);
    // PAD_CONFIG->PORT_1_CFG |= (1 << (2 * 9)); // Установка порта 1 в режим GPIO
	// GPIO_1->DIRECTION_IN = 1 << 9; // Установка направления порта в вход
    /***************************************************************************************/

    /**************************Включить вывод Input1 для Timer16_1**************************/
    /* Port0.8 */
    PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << 2 * TIMER16_1_IN1);
    /***************************************************************************************/

    /**************************Включить вывод Input2 для Timer16_1**************************/
    /* Port0.9 */
    // PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << 2 * TIMER16_1_IN2);
    /***************************************************************************************/

    /**************************Включить вывод Output для Timer16_1**************************/
    /* Port0.10 */
    PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << (2 * TIMER16_1_OUT)); 
    /***************************************************************************************/

    Timer16_1_Init();

    /* Задать значение для сравнения */
    //HAL_Timer16_SetCMP(&htimer16_1, 0xFFFF/2); 

    /*****************Запуск таймера в одиночном или продолжительном режиме*****************/
    HAL_Timer16_StartSingleMode(&htimer16_1);
    //HAL_Timer16_StartContinuousMode(&htimer16_1);
    /***************************************************************************************/

    /********************************Генерация волновой формы********************************/
    //HAL_Timer16_StartPWM(&htimer16_1, 0xFFFF, 0xFFFF/2);
    //HAL_Timer16_StartOneShot(&htimer16_1, 0xFFFF, 0xFFFF/2);
    //HAL_Timer16_StartSetOnes(&htimer16_1, 0xFFFF, 0xFFFF/2);
    /****************************************************************************************/

    /* Ожидание флага триггера */
    // HAL_Timer16_WaitTrigger(&htimer16_1);
    
    while (1)
    {    
        /* Очистка флага триггера */
        // HAL_Timer16_ClearTriggerFlag(&htimer16_1);
        
        /* Вывод значения счетчика */
        xprintf("Counter = %d\n", HAL_Timer16_GetCounterValue(&htimer16_1));

        /* Ожидание совпадения с CMP */
        // if(HAL_Timer16_CheckCMP(&htimer16_1))
        // {
        //     HAL_Timer16_ClearCMPFlag(&htimer16_1);
        //     xprintf("CMP\n");
        // }
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

    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_1.CountMode = TIMER16_COUNTMODE_EXTERNAL;  /* При тактировании от Input1 не имеет значения */
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_1;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактировании от Input1 */

    /* Настройка верхнего предела счета */
    htimer16_1.Period = 0xFFFF;
    /* Настройка режима обновления регистра ARR и CMP */
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;

    /* Настройка триггера */
    htimer16_1.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    htimer16_1.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;    /* При использовании триггера значение должно быть отлично от software */
    htimer16_1.Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;   /* Разрешить повторное срабатывание триггера */

    /* Настройки фильтра */
    htimer16_1.Filter.ExternalClock = TIMER16_FILTER_NONE;
    htimer16_1.Filter.Trigger = TIMER16_FILTER_NONE;

    /* Настройка режима энкодера */
    htimer16_1.EncoderMode = TIMER16_ENCODER_DISABLE;

    HAL_Timer16_Init(&htimer16_1);
}
