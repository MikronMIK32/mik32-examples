#include "mik32_hal_rcc.h"
#include "mik32_hal_timer16.h"
#include "mik32_hal_irq.h"

#include <gpio.h>

#include "uart_lib.h"
#include "xprintf.h"

/* Тип платы */
#define BOARD_LITE_V0

#ifdef BOARD_LITE_V0
#define PIN_LED 	7 // Светодиод управляется выводом PORT_2_7
#define PIN_BUTTON 	6 // Кнопка управляет сигналом на выводе PORT_2_6
#endif 


Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);


int main()
{    

    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    /* Настройка вывода светодиода */
    #ifdef BOARD_LITE_V0
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_LED)); 	// Установка вывода 7 порта 2 в режим GPIO
	GPIO_2->DIRECTION_OUT = 1 << PIN_LED; 				// Установка направления вывода 7 порта 2 на выход
	#endif 

    /************************Включить GPIO1.9 для триггера Timer16_1************************/
    PAD_CONFIG->PORT_1_CFG |= (1 << (2 * 9)); // Установка вывода 9 порта 1 в режим GPIO
	GPIO_1->DIRECTION_IN = 1 << 9; // Установка направления вывода 9 порта 1 на вход

    #ifdef BOARD_LITE_V0
    /* Настройка Gpio2.6 на выход. При соединении Port2.6 и Port1.9 нажатие пользовательской кнопки активирует триггер */
    PAD_CONFIG->PORT_2_CFG |= (1 << (2 * 6)); // Установка вывода 6 порта 2 в режим GPIO
	GPIO_2->DIRECTION_OUT = 1 << 6; // Установка направления вывода 6 порта 2 на выход
	#endif 
    /***************************************************************************************/

    /**************************Включить вывод Input1 для Timer16_1**************************/
    /* Port0.8 */
    // PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << 2 * TIMER16_1_IN1);
    /***************************************************************************************/

    /**************************Включить вывод Input2 для Timer16_1**************************/
    /* Port0.9 */
    // PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << 2 * TIMER16_1_IN2);
    /***************************************************************************************/

    /**************************Включить вывод Output для Timer16_1**************************/
    /* Port0.10 */
    // PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << (2 * TIMER16_1_OUT)); 
    /***************************************************************************************/

    Timer16_1_Init();

    /* Задать значение для сравнения */
    HAL_Timer16_SetCMP(&htimer16_1, htimer16_1.Period/2); 

    /* Включать прерывания Timer16 рекомендуется после его инициализации */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_1_MASK); 
    HAL_IRQ_EnableInterrupts();

    /*****************Запуск таймера в одиночном или продолжительном режиме*****************/
    // HAL_Timer16_StartSingleMode(&htimer16_1);
    HAL_Timer16_StartContinuousMode(&htimer16_1);
    /***************************************************************************************/

    /********************************Генерация волновой формы********************************/
    /* При использовании данных функций совместно с прерываниями ARROK и CMPOK наличие обработчика TIMER16_1_IRQHandler 
        и сброса соответствующих флагов обязательно */
    // HAL_Timer16_StartPWM(&htimer16_1, 0xFFFF, 0xFFFF/2);
    // HAL_Timer16_StartOneShot(&htimer16_1, 0xFFFF, 0xFFFF/2);
    // HAL_Timer16_StartSetOnes(&htimer16_1, 0xFFFF, 0xFFFF/2);
    /****************************************************************************************/

    uint32_t last_counter = 0;
    uint32_t counter = 0;
    
    while (1)
    {    
        counter = HAL_Timer16_GetCounterValue(&htimer16_1);
        
        if(counter != last_counter)
        {
            last_counter = counter;
            /* Вывод значения счетчика */
            xprintf("Counter = %d\n", counter);
        }
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
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_EPIC_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M | PM_CLOCK_TIMER16_1_M | PM_CLOCK_GPIO_0_M | PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_2_M;    
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Timer16_1_Init(void)
{
    htimer16_1.Instance = TIMER16_1;

    /* Настройка тактирования */
    htimer16_1.Clock.Source = TIMER16_SOURCE_INTERNAL_OSC32M;
    htimer16_1.CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_128;
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

    /* Разрешение прерываний */
    htimer16_1.Interrupts.DOWN = TIMER16_DOWN_IRQ_DISABLE;
    htimer16_1.Interrupts.UP = TIMER16_UP_IRQ_DISABLE;
    htimer16_1.Interrupts.ARROK = TIMER16_ARROK_IRQ_ENABLE;
    htimer16_1.Interrupts.CMPOK = TIMER16_CMPOK_IRQ_ENABLE;
    htimer16_1.Interrupts.EXTTRIG = TIMER16_EXTTRIG_IRQ_DISABLE;
    htimer16_1.Interrupts.ARRM = TIMER16_ARRM_IRQ_DISABLE;
    htimer16_1.Interrupts.CMPM = TIMER16_CMPM_IRQ_ENABLE;
    HAL_Timer16_InterruptInit(&htimer16_1);

    HAL_Timer16_Init(&htimer16_1);
}

void TIMER16_1_IRQHandler()
{
    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_DOWN_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nDOWN_IRQ\n");

        /* code */

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_DOWN_IRQ); /* Сброс флага прерывания */
    }

    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_UP_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nUP_IRQ\n");

        /* code */

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_UP_IRQ); /* Сброс флага прерывания */
    }

    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_ARROK_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nARROK_IRQ\n");

        /* code */

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_ARROK_IRQ); /* Сброс флага прерывания */
    }

    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_CMPOK_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nCMPOK_IRQ\n");

        /* code */

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_CMPOK_IRQ); /* Сброс флага прерывания */
    }  

    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_EXTTRIG_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nEXTTRIG_IRQ\n");

        /* code */

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_EXTTRIG_IRQ); /* Сброс флага прерывания */
    }  

    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_ARRM_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nARRM_IRQ\n");

        /* code */

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_ARRM_IRQ); /* Сброс флага прерывания */
    }  

    if (HAL_Timer16_GetInterruptStatus(&htimer16_1, TIMER16_CMPM_IRQ, TIMER16_IRQ_MASK_ENABLE))
    {
        xprintf("\nCMPM_IRQ\n");

        #ifdef BOARD_LITE_V0
        GPIO_2->OUTPUT ^= 1 << PIN_LED;   // Установка сигнала вывода 7 порта 2 в высокий уровень
        #endif 

        // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_CMPM_IRQ); /* Сброс флага прерывания */
    }  

    HAL_Timer16_SetClearInterruptMask(&htimer16_1, 0xFFFFFFFF); /* Сброс нескольких флагов прерывания */

}