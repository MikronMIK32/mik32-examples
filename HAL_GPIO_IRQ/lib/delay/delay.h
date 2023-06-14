#include <timer32.h>


void timer32_stop(TIMER32_TypeDef *timer);

void timer32_init(TIMER32_TypeDef *timer, uint32_t top, uint32_t prescaler, uint32_t control, uint32_t intmask);

void timer32_deinit(TIMER32_TypeDef *timer);

void timer32_start(TIMER32_TypeDef *timer);

void timer32_delay(TIMER32_TypeDef *timer, uint32_t delay_ms);

void timer32_delay_us(TIMER32_TypeDef *timer, uint32_t delay_us);
