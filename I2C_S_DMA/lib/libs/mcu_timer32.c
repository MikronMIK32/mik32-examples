#include <mcu_timer32.h>

void Timer_Delay(uint32_t periodMs) {
	DELAY_TIMER->Enable &= ~(TIMER32_ENABLE_M);
	DELAY_TIMER->Enable = TIMER32_RESET_VALUE_M;
	DELAY_TIMER->IntClear = TIMER32_INT_OVERFLOW_M;

	DELAY_TIMER->Top = periodMs * FREQ_BY_MS;
	DELAY_TIMER->IntMask = TIMER32_INT_OVERFLOW_M;
	DELAY_TIMER->Enable = TIMER32_ENABLE_M;
	while (DELAY_TIMER->IntFlags == 0);

	DELAY_TIMER->Enable &= ~(TIMER32_ENABLE_M);
	DELAY_TIMER->Enable = TIMER32_RESET_VALUE_M;
	DELAY_TIMER->IntClear = TIMER32_INT_OVERFLOW_M;
}

void Timer_Init(TIMER32_TypeDef* timer, uint32_t top, uint32_t prescale,
		Timer32_CountMode count_mode, Timer32_Source source) {
	timer->Enable &= ~(TIMER32_ENABLE_M);
	timer->Enable = TIMER32_RESET_VALUE_M;
	timer->Channels[0].Control = 0;
	timer->Channels[1].Control = 0;
	timer->Channels[2].Control = 0;
	timer->Channels[3].Control = 0;
	timer->IntClear = 0xFFFF;
	timer->Top = top;
	timer->Prescaler = prescale;
	timer->Control = count_mode | (source << 2);


}

void Timer_DeInit(TIMER32_TypeDef* timer) {
	timer->Enable &= ~(TIMER32_ENABLE_M);
	timer->Enable = TIMER32_RESET_VALUE_M;
	timer->Channels[0].Control = 0;
	timer->Channels[1].Control = 0;
	timer->Channels[2].Control = 0;
	timer->Channels[3].Control = 0;
	timer->IntClear = 0xFFFF;
	timer->Top = 0;
	timer->Prescaler = 0;
	timer->Control = 0;
}

void Timer_Reset(TIMER32_TypeDef* timer) {
	timer->Enable = TIMER32_RESET_VALUE_M;
}

void Timer_Start(TIMER32_TypeDef* timer) {
	timer->Enable = TIMER32_ENABLE_M;
}

void Timer_StartIT(TIMER32_TypeDef* timer, Timer32_InterruptMode intMode) {
	timer->IntMask = intMode & (TIMER_MODE_BIT_OVF | TIMER_MODE_BIT_UDF);

	timer->Enable = TIMER32_ENABLE_M;
}

void Timer_Stop(TIMER32_TypeDef* timer) {
	timer->Enable &= ~(TIMER32_ENABLE_M);
}
