#include "delay.h"

void timer32_stop(TIMER32_TypeDef *timer)
{
    timer->Enable = TIMER32_DISABLE_M;
    timer->IntClear = 0xFFFFFFFF;
}

void timer32_init(TIMER32_TypeDef *timer, uint32_t top, uint32_t prescaler, uint32_t control, uint32_t intmask)
{
    timer32_stop(timer);

    timer->Top = top;
    timer->Prescaler = prescaler;
    timer->Control = control;
    timer->IntMask = intmask;
}

void timer32_deinit(TIMER32_TypeDef *timer)
{
    // timer->Enable = TIMER32_RESET_VALUE_M | TIMER32_ENABLE_M;
    timer->IntMask = 0;
    timer->Prescaler = 0;
    timer->IntClear = 0xFFFFFFFF;
    timer->Enable = TIMER32_RESET_VALUE_M;
}

void timer32_start(TIMER32_TypeDef *timer)
{
    timer->Enable = TIMER32_ENABLE_M | TIMER32_RESET_VALUE_M;
}

void timer32_delay(TIMER32_TypeDef *timer, uint32_t delay_ms)
{
    timer32_init(timer, delay_ms * 32000, 0, TIMER32_CONTROL_MODE_UP_M, 1);
    timer32_start(timer);
    while ((timer->IntFlags && 1) == 0)
    {
    }
    timer32_deinit(timer);
}

void timer32_delay_us(TIMER32_TypeDef *timer, uint32_t delay_us)
{
    timer32_init(timer, delay_us * 32, 0, TIMER32_CONTROL_MODE_UP_M, 1);
    timer32_start(timer);
    while ((timer->IntFlags && 1) == 0)
    {
    }
    timer32_deinit(timer);
}
