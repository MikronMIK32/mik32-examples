#include "mik32_hal_timer16.h"

#include "uart_lib.h"
#include "xprintf.h"


Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);
void GPIO_Init();

int main()
{    

    SystemClock_Config();

    GPIO_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    /**************************Включить вывод Output для Timer16_1**************************/
    // /* Port0.10 */
    // PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << (2 * TIMER16_1_OUT)); 

    Timer16_1_Init();

    /* Задать значение для сравнения */
    //HAL_Timer16_SetCMP(&htimer16_1, 0xFFFF/2); 

    /*****************Запуск таймера в одиночном или продолжительном режиме*****************/
    // HAL_Timer16_StartSingleMode(&htimer16_1);
    HAL_Timer16_StartContinuousMode(&htimer16_1);
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
    PCC_OscInitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_OSC32K | PCC_OSCILLATORTYPE_OSC32M;   
    PCC_OscInit.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;                          
    PCC_OscInit.AHBDivider = 0;                             
    PCC_OscInit.APBMDivider = 0;                             
    PCC_OscInit.APBPDivider = 0;                             
    PCC_OscInit.HSI32MCalibrationValue = 0;                  
    PCC_OscInit.LSI32KCalibrationValue = 0;
    PCC_OscInit.RTCClockSelection = PCC_RTCCLKSOURCE_NO_CLK;
    PCC_OscInit.RTCClockCPUSelection = PCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_PCC_OscConfig(&PCC_OscInit);
}

static void Timer16_1_Init(void)
{
    htimer16_1.Instance = TIMER16_1;

    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
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

void GPIO_Init()
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
}