#include "mik32_hal.h"
#include "mik32_hal_timer16.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"


Timer16_HandleTypeDef htimer16_1;

void SystemClock_Config(void);
static void Timer16_1_Init(void);
void GPIO_Init();

/*
 * Пример для платы BOARD_LITE
 * В данном примере демонстрируется работа прерываний TIMER16_1. 
 * Пользовательская кнопка на выводе PORT2_6 соединена с выводом PORT1_9, фронт на котором 
 * является триггером для запуска отсчета TIMER16. При достижении значения сравнения или автозагрузки
 * срабатывает прерывание. Светодиод, который подключен к выводу PORT2_7, меняет свое состояние при 
 * совпадении счетчика со значением сравнения.
 *
 * */

int main()
{    
    HAL_Init();
    
    SystemClock_Config();

    GPIO_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Timer16_1_Init();

    /* Задать значение для сравнения */
    HAL_Timer16_SetCMP(&htimer16_1, htimer16_1.Period/2); 

    /* Включать прерывания Timer16 рекомендуется после его инициализации */
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER16_1_MASK); 
    HAL_IRQ_EnableInterrupts();

    /*****************Запуск таймера в одиночном или продолжительном режиме*****************/
    HAL_Timer16_StartSingleMode(&htimer16_1); /* Одиночный режим */
    // HAL_Timer16_StartContinuousMode(&htimer16_1); /* Продолжительный режим */
    /***************************************************************************************/

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
    htimer16_1.Clock.Prescaler = TIMER16_PRESCALER_128;
    htimer16_1.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактировании от Input1 */

    /* Настройка верхнего предела счета */
    htimer16_1.Period = 0xFFFF;
    /* Настройка режима обновления регистра ARR и CMP */
    htimer16_1.Preload = TIMER16_PRELOAD_AFTERWRITE;

    /* Настройка триггера */
    htimer16_1.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9; 
    htimer16_1.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_RISING;    /* При использовании триггера значение должно быть отлично от software */
    htimer16_1.Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;   /* Разрешить повторное срабатывание триггера */

    /* Настройки фильтра */
    htimer16_1.Filter.ExternalClock = TIMER16_FILTER_NONE;
    htimer16_1.Filter.Trigger = TIMER16_FILTER_NONE;

    /* Настройка режима энкодера */
    htimer16_1.EncoderMode = TIMER16_ENCODER_DISABLE;

    /* Разрешение прерываний */
    htimer16_1.Interrupts.DOWN = TIMER16_DOWN_IRQ_DISABLE;
    htimer16_1.Interrupts.UP = TIMER16_UP_IRQ_DISABLE;
    htimer16_1.Interrupts.ARROK = TIMER16_ARROK_IRQ_DISABLE;
    htimer16_1.Interrupts.CMPOK = TIMER16_CMPOK_IRQ_DISABLE;
    htimer16_1.Interrupts.EXTTRIG = TIMER16_EXTTRIG_IRQ_ENABLE;
    htimer16_1.Interrupts.ARRM = TIMER16_ARRM_IRQ_ENABLE;
    htimer16_1.Interrupts.CMPM = TIMER16_CMPM_IRQ_ENABLE;
    HAL_Timer16_InterruptInit(&htimer16_1);

    HAL_Timer16_Init(&htimer16_1);
}

void trap_handler()
{
    if (EPIC_CHECK_TIMER16_1())
    {
        uint32_t interrupt_status = HAL_Timer16_GetInterruptStatus(&htimer16_1);

        if (interrupt_status & TIMER16_ISR_DOWN_M)
        {
            xprintf("\nDOWN_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_DOWN_IRQ); /* Сброс флага прерывания */
        }

        if (interrupt_status & TIMER16_ISR_UP_M)
        {
            xprintf("\nUP_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_UP_IRQ); /* Сброс флага прерывания */
        }

        if (interrupt_status & TIMER16_ISR_ARR_OK_M)
        {
            xprintf("\nARROK_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_ARROK_IRQ); /* Сброс флага прерывания */
        }

        if (interrupt_status & TIMER16_ISR_CMP_OK_M)
        {
            xprintf("\nCMPOK_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_CMPOK_IRQ); /* Сброс флага прерывания */
        }  

        if (interrupt_status & TIMER16_ISR_EXT_TRIG_M)
        {
            xprintf("\nEXTTRIG_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_EXTTRIG_IRQ); /* Сброс флага прерывания */
        }  

        if (interrupt_status & TIMER16_ISR_ARR_MATCH_M)
        {
            xprintf("\nARRM_IRQ\n");
            /* code */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_ARRM_IRQ); /* Сброс флага прерывания */
        }  

        if (interrupt_status & TIMER16_ISR_CMP_MATCH_M)
        {
            xprintf("\nCMPM_IRQ\n");
            HAL_GPIO_TogglePin(GPIO_2, PORT2_7); /* Смена сигнала PORT2_7 на противоположный */
            // HAL_Timer16_ClearInterruptFlag(&htimer16_1, TIMER16_CMPM_IRQ); /* Сброс флага прерывания */
        }  

        HAL_Timer16_SetClearInterruptMask(&htimer16_1, 0xFFFFFFFF); /* Сброс нескольких флагов прерываний по маске */
    }


    /* Сброс прерываний */
    HAL_EPIC_Clear(0xFFFFFFFF);


    #ifdef MIK32_IRQ_DEBUG
        xprintf("Clear\n");
        xprintf("EPIC->RAW_STATUS = %d\n", EPIC->RAW_STATUS);
        xprintf("EPIC->STATUS = %d\n", EPIC->STATUS);
    #endif
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

    GPIO_InitStruct.Pin = PORT2_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PORT1_9;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PORT2_6;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
}