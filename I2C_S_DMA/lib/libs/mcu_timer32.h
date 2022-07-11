#ifndef MCU_TIMER32_H_INCLUDED
#define MCU_TIMER32_H_INCLUDED

#include "mcu32_memory_map.h"
#include "timer32.h"

#include <stdint.h>

#include "mcu_core.h"

/** Число тактов за миллисекунду, используется в Timer_Delay для задания
 * максимального значения таймера
 */
#define FREQ_BY_MS (FREQ/1000)

/** Выбор таймера для функции задержки Timer_Delay
 * \note TIMER32_0 Используется для задержки в функции Timer_Delay,
 * если планируете использовать задержку на таймере, не используете эту функцию
 */
#define DELAY_TIMER (TIMER32_0)

/** Функция задержки на таймере TIMER32
 *
 * \param periodMs Время задержки в миллисекундах
 */
void Timer_Delay(uint32_t periodMs);


typedef enum {
	TIMER_COUNT_FORWARD = 0,
	TIMER_COUNT_REVERSE = 1,
	TIMER_COUNT_BIDIRECTIONAL = 2,
} Timer32_CountMode;

/** Выбор источника тактового сигнала для счета таймера
 *
 */
typedef enum {
	TIMER_SOURCE_PRESCALER = 0,/** Выход предделителя */
	TIMER_SOURCE_TIM1 = 1,     /** Вход модуля TIM1 */
	TIMER_SOURCE_TIMER_TX = 2, /** Вход микросхемы timerX_Tx_pin_pad (X – номер таймера) */
	TIMER_SOURCE_TIM2 = 3,     /** Вход модуля TIM2 */
} Timer32_Source;

/** Инициализация таймера
 *
 * \param timer Выбор таймера, TIMER32_X (X - номер таймера)
 * \param top Максимальное значение счетчика, ограничивает сверху
 * \param prescale Значение предварительного делителя. Предделитель вырабатывает
 *  тактовый сигнал для счета, частота которого в целое раз меньше входной
 * \param count_mode Режим счета таймера, TIMER_COUNT_FORWARD - прямой режим счета
 * \param source Источник тактового сигнала, TIMER_SOURCE_PRESCALER - источник -
 *  предделитель
 */
void Timer_Init(TIMER32_TypeDef* timer, uint32_t top, uint32_t prescale,
		Timer32_CountMode count_mode, Timer32_Source source);

/** Функция деинициализации таймера, выключает таймер и сбрасывает его настройки
 *
 * \param timer Выбор таймера, TIMER32_X (X - номер таймера)
 */
void Timer_DeInit(TIMER32_TypeDef* timer);

/** Сброс счетчика таймера
 *
 * \param timer Выбор таймера, TIMER32_X (X - номер таймера)
 */
void Timer_Reset(TIMER32_TypeDef* timer);

/** Включение счета таймера
 *
 * \note Функция не сбрасывает значение счетчика!
 *
 * \param timer Выбор таймера, TIMER32_X (X - номер таймера)
 */
void Timer_Start(TIMER32_TypeDef* timer);

#define TIMER_MODE_BIT_OVF 1 //1 if Overflow
#define TIMER_MODE_BIT_UDF 2 //1 if Underflow

/** Режим прерывания таймера
 * \note В режиме события (EVENT) функция инициализации не включает прерывания
 *  таймера в EPIC
 */
typedef enum {
	TIMER_MODE_INT_UNDERFLOW = 1,  /** Режим прерывания по переполнению счетчика */
	TIMER_MODE_INT_OVERFLOW = 2,   /** Режим прерывания по опустошению счетчика */
} Timer32_InterruptMode;

/** Включение счета таймера и прерывания
 *
 * \note Функция не сбрасывает значение счетчика!
 *
 * \param timer Выбор таймера, TIMER32_X (X - номер таймера)
 * \param intMode Выбор режима прерывания
 */
void Timer_StartIT(TIMER32_TypeDef* timer, Timer32_InterruptMode intMode);

/** Остановка таймера
 *
 * \note Функция не сбрасывает значение счетчика!
 *
 * \param timer Выбор таймера, TIMER32_X (X - номер таймера)
 */
void Timer_Stop(TIMER32_TypeDef* timer);

/** Функция получения состояния линии прерывания
 *
 * \param irq_line Номер линии прерывания
 * \return Возвращает true если сработало прерывание данной линии
 */
bool Timer_LineInterruptState();

/** Функция чтения логического уровня вывода, подключенного к линии прерывания
 *
 * \param irq_line Номер линии прерывания
 * \return Логический уровень вывода
 */
int Timer_LinePinState();

/** Функция сброса регистра состояния прерываний.
 *  \note Когда срабатывает прерывание на одной из линии, в регистре INT_FLAG
 *  выставляется 1 в разряде, соответствующем линии прерывания.
 *  После обработки прерывания необходимо сбросить данный регистр
 *  в обработчике прерывания GPIO_IRQ_TRAP_HANDLER().
 *  Если после обработки прерывания регистр не был сброшен,
 *  обработчик будет вызван снова, программа будет бесконечно вызывать обработчик.
 */
void Timer0_ClearInterrupt();

/** Функция включения прерываний ввода-вывода
 *  \note Функция включает данный вид прерываний в EPIC
 */
void Timer_EnableInterrupts(TIMER32_TypeDef* timer);

/** Функция отключения прерываний ввода-вывода
*   \note Функция включает данный вид прерываний в EPIC
*/
void Timer_DisableInterrupts(TIMER32_TypeDef* timer);

#endif
