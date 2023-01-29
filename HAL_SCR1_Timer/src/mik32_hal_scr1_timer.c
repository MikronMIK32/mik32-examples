#include "mik32_hal_scr1_timer.h"

void HAL_SCR1_Timer_Enable(OTP_HandleTypeDef *hscr1_timer)
{
    hscr1_timer->Instance->TIMER_CTRL |= SCR1_TIMER_ENABLE_M;
}

void HAL_SCR1_Timer_Disable(OTP_HandleTypeDef *hscr1_timer)
{
    hscr1_timer->Instance->TIMER_CTRL &= ~SCR1_TIMER_ENABLE_M;
}

void HAL_SCR1_Timer_SetClockSource(OTP_HandleTypeDef *hscr1_timer, uint8_t ClockSource)
{
    hscr1_timer->ClockSource = ClockSource;

    switch (ClockSource)
    {
    case SCR1_TIMER_CLKSRC_INTERNAL:
        hscr1_timer->Instance->TIMER_CTRL &= SCR1_TIMER_CLKSRC_INTERNAL_M;
        break;
    
    case SCR1_TIMER_CLKSRC_EXTERNAL_RTC:
        hscr1_timer->Instance->TIMER_CTRL |= SCR1_TIMER_CLKSRC_RTC_M;
        break;
    }
}

void HAL_SCR1_Timer_SetDivider(OTP_HandleTypeDef *hscr1_timer, uint16_t Divider)
{
    /* Divider 10-битное число */
    if(Divider > 1023)
    {
        Divider = 1023;
    }
    hscr1_timer->Divider = Divider;
    hscr1_timer->Instance->TIMER_DIV = Divider;
}

void HAL_SCR1_Timer_Init(OTP_HandleTypeDef *hscr1_timer)
{
    hscr1_timer->Instance->TIMER_CTRL = 0;

    /* Настройка источника тактирования */
    HAL_SCR1_Timer_SetClockSource(hscr1_timer, hscr1_timer->ClockSource);

    /* Настройка делителя */
    HAL_SCR1_Timer_SetDivider(hscr1_timer, hscr1_timer->Divider);

}

void HAL_DelayMs(OTP_HandleTypeDef *hscr1_timer, uint32_t Milliseconds)
{

    uint8_t AHBMDivider = PM->DIV_AHB;          /* Делитель частоты */
    uint32_t Milliseconds_max_32bit = 134217;   /* 134217 - максимальное число миллисекунд для того чтобы при расчете количества тактов не превысить 32 бита */

    if (Milliseconds > Milliseconds_max_32bit)
    {
        Milliseconds = Milliseconds_max_32bit;  /* Задержка равна предельному значению 134217, мс */
    }

    uint32_t Ticks = Milliseconds * (MIK32_FREQ/1000);  /* Количество тактов системного таймера для достижения значения Milliseconds */
    Ticks = Ticks / (AHBMDivider + 1);
    Ticks = Ticks / (hscr1_timer->Divider + 1);

    #ifdef MIK32_SCR1_TIMER_DEBUG
    xprintf("Milliseconds = %u\n", Milliseconds);
    xprintf("Ticks = %u\n", Ticks);
    #endif
    

    hscr1_timer->Instance->MTIME = 0;
    hscr1_timer->Instance->MTIMEH = 0;

    hscr1_timer->Instance->MTIMECMP = Ticks;      
    hscr1_timer->Instance->MTIMECMPH = 0;

    HAL_SCR1_Timer_Enable(hscr1_timer);

    while ((hscr1_timer->Instance->MTIME) < Ticks);

    HAL_SCR1_Timer_Disable(hscr1_timer);

    hscr1_timer->Instance->MTIME = 0;
    hscr1_timer->Instance->MTIMEH = 0;
    
}



