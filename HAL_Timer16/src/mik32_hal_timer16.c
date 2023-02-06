#include "mik32_hal_timer16.h"


void HAL_Timer16_Disable(Timer16_HandleTypeDef *htimer16)
{
    htimer16->Instance->CR &= ~TIMER16_CR_ENABLE_M;
}

void HAL_Timer16_Enable(Timer16_HandleTypeDef *htimer16)
{
    htimer16->Instance->CR |= TIMER16_CR_ENABLE_M;
}

void HAL_Timer16_SetPad(Timer16_HandleTypeDef *htimer16)
{
    uint32_t Port0_config = PAD_CONFIG->PORT_0_CFG;
    if(htimer16->Instance == TIMER16_0)
    {
        /* Смена режима вывода */
        Port0_config &= ~(0b11 << 2 * TIMER16_0_IN1);
        Port0_config |= (PORT_AS_TIMER << 2 * TIMER16_0_IN1);

        // Port0_config &= ~(0b11 << 2 * TIMER16_0_IN2);
        // Port0_config |= (PORT_AS_TIMER << 2 * TIMER16_0_IN2);
    }
    else if (htimer16->Instance == TIMER16_1)
    {
        /* Смена режима вывода */
        Port0_config &= ~(0b11 << 2 * TIMER16_1_IN1);
        Port0_config |= (PORT_AS_TIMER << 2 * TIMER16_1_IN1);

        // Port0_config &= ~(0b11 << 2 * TIMER16_1_IN2);
        // Port0_config |= (PORT_AS_TIMER << 2 * TIMER16_1_IN2);
    }
    else if (htimer16->Instance == TIMER16_2)
    {
        /* Смена режима вывода */
        Port0_config &= ~(0b11 << 2 * TIMER16_2_IN1);
        Port0_config |= (PORT_AS_TIMER << 2 * TIMER16_2_IN1);

        // Port0_config &= ~(0b11 << 2 * TIMER16_2_IN2);
        // Port0_config |= (PORT_AS_TIMER << 2 * TIMER16_2_IN2);
    }

    PAD_CONFIG->PORT_0_CFG = Port0_config;
}

void HAL_Timer16_SetActiveEdge(Timer16_HandleTypeDef *htimer16, uint8_t ActiveEdge)
{
    htimer16->ActiveEdge = ActiveEdge;

    HAL_Timer16_Disable(htimer16);

    htimer16->Instance->CFGR &= ~TIMER16_CFGR_CKPOL_M;
    htimer16->Instance->CFGR |= ActiveEdge << TIMER16_CFGR_CKPOL_S;
}

void HAL_Timer16_SetSourceClock(Timer16_HandleTypeDef *htimer16, uint8_t SourceClock)
{
    htimer16->Clock.Source = SourceClock;

    HAL_Timer16_Disable(htimer16);

    if(SourceClock == TIMER16_SOURCE_EXTERNAL_INPUT1)    /* Внешний источник Input1 */
    {
        htimer16->Instance->CFGR |= TIMER16_CFGR_CKSEL_M;  

        HAL_Timer16_SetActiveEdge(htimer16, htimer16->ActiveEdge);  /* Настройка активного фронта при тактировании от внешнего источника  */

        HAL_Timer16_SetPad(htimer16); /* Настройка режима вывода */
    }
    else    /* Внутренний источник */
    {
        /* Настройка источника тактирования таймера в PM(Power Manager) */
        uint32_t CFGConfig = PM->TIMER_CFG;
        if(htimer16->Instance == TIMER16_0)
        {
            xprintf("Timer16_0\n");

            CFGConfig &= ~PM_TIMER_CFG_M(MUX_TIM16_0);
            CFGConfig |= SourceClock << MUX_TIM16_0;
        }
        else if (htimer16->Instance == TIMER16_1)
        {
            xprintf("Timer16_1\n");

            CFGConfig &= ~PM_TIMER_CFG_M(MUX_TIM16_1);
            CFGConfig |= SourceClock << MUX_TIM16_1;
        }
        else if (htimer16->Instance == TIMER16_2)
        {
            xprintf("Timer16_2\n");

            CFGConfig &= ~PM_TIMER_CFG_M(MUX_TIM16_2);
            CFGConfig |= SourceClock << MUX_TIM16_2;
        }
        /* Установка выбранного источника тактирования таймера в PM */
        PM->TIMER_CFG = CFGConfig;
        
        htimer16->Instance->CFGR &= ~TIMER16_CFGR_CKSEL_M;  /* Внутренний источник */
    }

}

void HAL_Timer16_SetCountMode(Timer16_HandleTypeDef *htimer16, uint8_t CountMode)
{
    htimer16->CountMode = CountMode;

    HAL_Timer16_Disable(htimer16);
    
    switch (CountMode)
    {
    case TIMER16_COUNTMODE_INTERNAL:
        htimer16->Instance->CFGR &= ~TIMER16_CFGR_COUNTMODE_M;
        break;
    case TIMER16_COUNTMODE_EXTERNAL:
        htimer16->Instance->CFGR |= TIMER16_CFGR_COUNTMODE_M;
        HAL_Timer16_SetPad(htimer16);
        break;
    }
        
}

void HAL_Timer16_ClockInit(Timer16_HandleTypeDef *htimer16)
{
    HAL_Timer16_SetSourceClock(htimer16, htimer16->Clock.Source);
    HAL_Timer16_SetCountMode(htimer16, htimer16->CountMode);
}

void HAL_Timer16_SetPreload(Timer16_HandleTypeDef *htimer16, uint8_t Preload)
{
    htimer16->Preload = Preload;

    HAL_Timer16_Disable(htimer16);

    switch (Preload)
    {
    case TIMER16_PRELOAD_AFTERWRITE:
        htimer16->Instance->CFGR &= ~TIMER16_CFGR_PRELOAD_M;
        break;
    case TIMER16_PRELOAD_ENDPERIOD:
        htimer16->Instance->CFGR |= TIMER16_CFGR_PRELOAD_M;
        break;
    }
}

void HAL_Timer16_WaitARROK(Timer16_HandleTypeDef *htimer16)
{
    while (!(htimer16->Instance->ISR & TIMER16_ISR_ARR_OK_M));
    htimer16->Instance->ICR |= TIMER16_ICR_ARROKCF_M;
    //while (htimer16->Instance->ISR & TIMER16_ISR_ARR_OK_M);
}

void HAL_Timer16_WaitCMPOK(Timer16_HandleTypeDef *htimer16)
{
    while (!(htimer16->Instance->ISR & TIMER16_ISR_CMP_OK_M));
    htimer16->Instance->ICR |= TIMER16_ICR_CMPOKCF_M;
    //while (htimer16->Instance->ISR & TIMER16_ISR_CMP_OK_M);
}

void HAL_Timer16_SetARR(Timer16_HandleTypeDef *htimer16, uint16_t Period)
{
    htimer16->Period = Period;

    /* Выключение таймера для записи ARR */
    //HAL_Timer16_Disable(htimer16);

    htimer16->Instance->ARR = Period;
    //HAL_Timer16_Enable(htimer16);
    HAL_Timer16_WaitARROK(htimer16);
}

void HAL_Timer16_SetCMP(Timer16_HandleTypeDef *htimer16, uint16_t Compare)
{
    /* Выключение таймера для записи CMP */

    //HAL_Timer16_Disable(htimer16);

    htimer16->Instance->CMP = Compare;
    //HAL_Timer16_Enable(htimer16);
    HAL_Timer16_WaitCMPOK(htimer16);
}

void HAL_Timer16_SelectTrigger(Timer16_HandleTypeDef *htimer16, uint8_t TriggerSource)
{
    htimer16->Trigger.Source = TriggerSource;

    /* Выключение таймера для записи CFGR */
    HAL_Timer16_Disable(htimer16);

    uint32_t CFGRConfig = htimer16->Instance->CFGR;

    CFGRConfig &= ~TIMER16_CFGR_TRIGSEL_M;
    CFGRConfig |= TriggerSource << TIMER16_CFGR_TRIGSEL_S;

    htimer16->Instance->CFGR = CFGRConfig;
}

void HAL_Timer16_SetTriggerEdge(Timer16_HandleTypeDef *htimer16, uint8_t TriggerEdge)
{
    htimer16->Trigger.ActiveEdge = TriggerEdge;

    /* Выключение таймера для записи CFGR */
    HAL_Timer16_Disable(htimer16);

    uint32_t CFGRConfig = htimer16->Instance->CFGR;

    CFGRConfig &= ~TIMER16_CFGR_TRIGEN_M;
    CFGRConfig |= TriggerEdge << TIMER16_CFGR_TRIGEN_S;

    htimer16->Instance->CFGR = CFGRConfig;
}

/**************************Режимы таймера***************************/
void HAL_Timer16_CounterModeInit(Timer16_HandleTypeDef *htimer16)
{
    HAL_Timer16_SetPreload(htimer16, htimer16->Preload);

    /* Настройка триггера */
    HAL_Timer16_SelectTrigger(htimer16, htimer16->Trigger.Source);
    HAL_Timer16_SetTriggerEdge(htimer16, htimer16->Trigger.ActiveEdge);

}
/*******************************************************************/

void HAL_Timer16_SetFilterExternalClock(Timer16_HandleTypeDef *htimer16, uint8_t FilterExternalClock)
{
    htimer16->Filter.ExternalClock = FilterExternalClock;

    /* Выключение таймера для записи в CFGR */
    HAL_Timer16_Disable(htimer16);

    htimer16->Instance->CFGR &= ~TIMER16_CFGR_CKFLT_M;
    htimer16->Instance->CFGR |= FilterExternalClock << TIMER16_CFGR_CKFLT_S;
}

void HAL_Timer16_SetFilterTrigger(Timer16_HandleTypeDef *htimer16, uint8_t FilterTrigger)
{
    htimer16->Filter.Trigger = FilterTrigger;

    /* Выключение таймера для записи в CFGR */
    HAL_Timer16_Disable(htimer16);

    htimer16->Instance->CFGR &= ~TIMER16_CFGR_TRGFLT_M;
    htimer16->Instance->CFGR |= FilterTrigger << TIMER16_CFGR_TRGFLT_S;
}

void HAL_Timer16_Init(Timer16_HandleTypeDef *htimer16)
{
    HAL_Timer16_Disable(htimer16);
    /* Настройка внутреннего/внешнего источника тактирования */
    HAL_Timer16_ClockInit(htimer16);

    HAL_Timer16_SetFilterExternalClock(htimer16, htimer16->Filter.ExternalClock);
    HAL_Timer16_SetFilterTrigger(htimer16, htimer16->Filter.Trigger);
    
    switch (htimer16->CountMode)
    {
    case TIMER16_MODE_COUNTER:
        HAL_Timer16_CounterModeInit(htimer16);
        break;
    case TIMER16_MODE_WAVE:
        //HAL_Timer16_CounterModeInit(htimer16);
        break;
    case TIMER16_MODE_TIMEOUT:
        //HAL_Timer16_CounterModeInit(htimer16);
        break;
    }

    HAL_Timer16_Enable(htimer16);

    /* Верхний предел счета */
    HAL_Timer16_SetARR(htimer16, htimer16->Period);

}

uint16_t HAL_Timer16_GetCounterValue(Timer16_HandleTypeDef *htimer16)
{
    return htimer16->Instance->CNT;
}

void HAL_Timer16_StartSingleMode(Timer16_HandleTypeDef *htimer16)
{
    htimer16->Instance->CR |= TIMER16_CR_SNGSTRT_M;
}

void HAL_Timer16_StartContinuousMode(Timer16_HandleTypeDef *htimer16)
{
    htimer16->Instance->CR |= TIMER16_CR_CNTSTRT_M;
}

void HAL_Timer16_InvertOutput(Timer16_HandleTypeDef *htimer16)
{
    //HAL_Timer16_Disable(htimer16);
    uint32_t CFGRConfig = htimer16->Instance->CFGR & TIMER16_CFGR_WAVPOL_M;
    CFGRConfig = CFGRConfig ^ TIMER16_CFGR_WAVPOL_M;
    CFGRConfig |= htimer16->Instance->CFGR & (~TIMER16_CFGR_WAVPOL_M);
    htimer16->Instance->CFGR = CFGRConfig;
    HAL_Timer16_Enable(htimer16);
}

void HAL_Timer16_OutConfig(Timer16_HandleTypeDef *htimer16)
{
    /* Настройка вывода Timer16_n_out в режим таймера16 */
    uint32_t CFGConfig = PAD_CONFIG->PORT_0_CFG;
    if(htimer16->Instance == TIMER16_0)
    {
        CFGConfig &= ~(0b11 << (2 * TIMER16_0_OUT)); 
        CFGConfig |= (PORT_AS_TIMER << (2 * TIMER16_0_OUT)); // Установка вывода порта 0 в режим GPIO
    }
    else if (htimer16->Instance == TIMER16_1)
    {
        CFGConfig &= ~(0b11 << (2 * TIMER16_1_OUT)); 
        CFGConfig |= (PORT_AS_TIMER << (2 * TIMER16_1_OUT)); // Установка вывода порта 0 в режим GPIO
    }
    else if (htimer16->Instance == TIMER16_2)
    {
        CFGConfig &= ~(0b11 << (2 * TIMER16_2_OUT)); 
        CFGConfig |= (PORT_AS_TIMER << (2 * TIMER16_2_OUT)); // Установка вывода порта 0 в режим GPIO
    }
    PAD_CONFIG->PORT_0_CFG = CFGConfig;
}

void HAL_Timer16_StartPWM(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare)
{
    HAL_Timer16_Disable(htimer16);
    htimer16->Instance->CFGR &= ~TIMER16_CFGR_WAVE_M;
    HAL_Timer16_Enable(htimer16);

    /* Настройка вывода Timer16_n_out в режим таймера16 */
    HAL_Timer16_OutConfig(htimer16);
    
    if(Period > Compare)
    {
        HAL_Timer16_SetCMP(htimer16, Compare);
        HAL_Timer16_SetARR(htimer16, Period);
    }

    HAL_Timer16_StartContinuousMode(htimer16);

}

void HAL_Timer16_StartOneShot(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare)
{
    HAL_Timer16_Disable(htimer16);
    htimer16->Instance->CFGR &= ~TIMER16_CFGR_WAVE_M;
    HAL_Timer16_Enable(htimer16);
    
    /* Настройка вывода Timer16_n_out в режим таймера16 */
    HAL_Timer16_OutConfig(htimer16);
    
    if(Period > Compare)
    {
        HAL_Timer16_SetCMP(htimer16, Compare);
        HAL_Timer16_SetARR(htimer16, Period);
    }

    HAL_Timer16_StartSingleMode(htimer16);

}

void HAL_Timer16_StartSetOnes(Timer16_HandleTypeDef *htimer16, uint16_t Period, uint16_t Compare)
{
    HAL_Timer16_Disable(htimer16);
    htimer16->Instance->CFGR |= TIMER16_CFGR_WAVE_M;
    HAL_Timer16_Enable(htimer16);
    
    /* Настройка вывода Timer16_n_out в режим таймера16 */
    HAL_Timer16_OutConfig(htimer16);
    
    if(Period > Compare)
    {
        HAL_Timer16_SetCMP(htimer16, Compare);
        HAL_Timer16_SetARR(htimer16, Period);
    }

    HAL_Timer16_StartSingleMode(htimer16);
}

void HAL_Timer16_ClearTriggerFlag(Timer16_HandleTypeDef *htimer16)
{
    htimer16->Instance->ICR |= TIMER16_ICR_EXTTRIGCF_M;
}

void HAL_Timer16_WaitTrigger(Timer16_HandleTypeDef *htimer16)
{
    while (!(htimer16->Instance->ISR & TIMER16_ISR_EXT_TRIG_M))
    {
        //xprintf("EXTTRIG = %d. Counter = %d\n", (htimer16->Instance->ISR & TIMER16_ISR_EXT_TRIG_M) >> TIMER16_ISR_EXT_TRIG_S, HAL_Timer16_GetCounterValue(htimer16));
    }
    HAL_Timer16_ClearTriggerFlag(htimer16);
}