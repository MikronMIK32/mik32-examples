#ifndef MCU_DIGITAL_H_INCLUDED
#define MCU_DIGITAL_H_INCLUDED

#include "mcu32_memory_map.h"
#include "gpio.h"

#include <stdbool.h>

#include "mcu_core.h"

/** Константы логического уровня вывода
 *
 */
typedef enum {
	GPIO_HIGH = 1, /**< Высокий уровень*/
	GPIO_LOW = 0   /**< Низкий уровень */
} GPIO_PinState;

/** Функция чтения логического уровня вывода
 *
 * \param gpio Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param gpioNum Номер вывода, принимает значения 0..15 для портов 0..1 и значения 0..7 для порта 2
 * \return Логический уровень вывода
 */
GPIO_PinState GPIO_PinRead(GPIO_TypeDef *gpio, uint32_t gpioNum);


/** Функция установки логического уровня вывода
 *
 * \param gpio Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param gpioNum Номер вывода, принимает значения 0..15 для портов 0..1 и значения 0..7 для порта 2
 * \param state Устанавливаемый логический уровень
 */
void GPIO_PinWrite(GPIO_TypeDef *gpio, uint32_t gpioNum, GPIO_PinState state);


/** Функция установки низкого логического уровня выводов по маске
 *
 * \param GPIO Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param MASK Маска выводов
 */
#define GPIO_PIN_MASK_CLEAR(GPIO, MASK)  ((GPIO)->CLEAR = (MASK))

/** Функция установки высокого логического уровня выводов по маске
 *
 * \param GPIO Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param MASK Маска выводов
 */
#define GPIO_PIN_MASK_SET(GPIO, MASK)  ((GPIO)->SET = (MASK))


/** Функция изменения логического уровня вывода на противоположный
 *
 * \param gpio Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param gpioNum Номер вывода, принимает значения 0..15 для портов 0..1 и значения 0..7 для порта 2
 */
void GPIO_PinToggle(GPIO_TypeDef *gpio, uint32_t gpioNum);


/** Константы направления вывода
 *
 */
typedef enum {
	GPIO_DIR_OUTPUT = 1, /**< Вывод установлен на выход*/
	GPIO_DIR_INPUT = 0   /**< Вывод установлен на вход*/
} GPIO_PinDirection;


/** Функция установки направления вывода
 *
 * \param gpio Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param gpioNum Номер вывода, принимает значения 0..15 для портов 0..1 и значения 0..7 для порта 2
 * \param dir Направление вывода
 */
void GPIO_SetPinDirection(GPIO_TypeDef *gpio, uint32_t gpioNum, GPIO_PinDirection dir);


/** Функция установки направления вывода по маске
 *
 * \param gpio \param gpio Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param mask Маска выводов
 * \param dir Направление выводов
 */
void GPIO_SetPinMaskDirection(GPIO_TypeDef *gpio, uint16_t mask, GPIO_PinDirection dir);


/**
 *
 */
typedef enum {
	GPIO_LINE_0 = 0,/**< Линия прерываний 0 */
	GPIO_LINE_1 = 1,/**< Линия прерываний 1 */
	GPIO_LINE_2 = 2,/**< Линия прерываний 2 */
	GPIO_LINE_3 = 3,/**< Линия прерываний 3 */
	GPIO_LINE_4 = 4,/**< Линия прерываний 4 */
	GPIO_LINE_5 = 5,/**< Линия прерываний 5 */
	GPIO_LINE_6 = 6,/**< Линия прерываний 6 */
	GPIO_LINE_7 = 7,/**< Линия прерываний 7 */
} GPIO_Line;

typedef enum {
	/*
	GPIO_MUX_GPIO_0_0_LINE_0 = 0,
	GPIO_MUX_GPIO_0_8_LINE_0 = 1,
	GPIO_MUX_GPIO_1_0_LINE_0 = 2,
	GPIO_MUX_GPIO_1_8_LINE_0 = 3,
	GPIO_MUX_GPIO_2_0_LINE_0 = 4,
	GPIO_MUX_GPIO_0_4_LINE_0 = 5,
	GPIO_MUX_GPIO_0_12_LINE_0 = 6,
	GPIO_MUX_GPIO_1_4_LINE_0 = 7,
	GPIO_MUX_GPIO_1_12_LINE_0 = 8,
	GPIO_MUX_GPIO_2_4_LINE_0 = 9,

	GPIO_MUX_GPIO_0_1_LINE_1 = 0,
	GPIO_MUX_GPIO_0_9_LINE_1 = 1,
	GPIO_MUX_GPIO_1_1_LINE_1 = 2,
	GPIO_MUX_GPIO_1_9_LINE_1 = 3,
	GPIO_MUX_GPIO_2_1_LINE_1 = 4,
	GPIO_MUX_GPIO_0_5_LINE_1 = 5,
	GPIO_MUX_GPIO_0_13_LINE_1 = 6,
	GPIO_MUX_GPIO_1_5_LINE_1 = 7,
	GPIO_MUX_GPIO_1_13_LINE_1 = 8,
	GPIO_MUX_GPIO_2_5_LINE_1 = 9,

	GPIO_MUX_GPIO_0_2_LINE_2 = 0,
	GPIO_MUX_GPIO_0_10_LINE_2 = 1,
	GPIO_MUX_GPIO_1_2_LINE_2 = 2,
	GPIO_MUX_GPIO_1_10_LINE_2 = 3,
	GPIO_MUX_GPIO_2_2_LINE_2 = 4,
	GPIO_MUX_GPIO_0_6_LINE_2 = 5,
	GPIO_MUX_GPIO_0_14_LINE_2 = 6,
	GPIO_MUX_GPIO_1_6_LINE_2 = 7,
	GPIO_MUX_GPIO_1_14_LINE_2 = 8,
	GPIO_MUX_GPIO_2_6_LINE_2 = 9,

	GPIO_MUX_GPIO_0_3_LINE_3 = 0,
	GPIO_MUX_GPIO_0_11_LINE_3 = 1,
	GPIO_MUX_GPIO_1_3_LINE_3 = 2,
	GPIO_MUX_GPIO_1_11_LINE_3 = 3,
	GPIO_MUX_GPIO_2_3_LINE_3 = 4,
	GPIO_MUX_GPIO_0_7_LINE_3 = 5,
	GPIO_MUX_GPIO_0_15_LINE_3 = 6,
	GPIO_MUX_GPIO_1_7_LINE_3 = 7,
	GPIO_MUX_GPIO_1_15_LINE_3 = 8,
	GPIO_MUX_GPIO_2_7_LINE_3 = 9,

	GPIO_MUX_GPIO_0_4_LINE_4 = 0,
	GPIO_MUX_GPIO_0_12_LINE_4 = 1,
	GPIO_MUX_GPIO_1_4_LINE_4 = 2,
	GPIO_MUX_GPIO_1_12_LINE_4 = 3,
	GPIO_MUX_GPIO_2_4_LINE_4 = 4,
	GPIO_MUX_GPIO_0_0_LINE_4 = 5,
	GPIO_MUX_GPIO_0_8_LINE_4 = 6,
	GPIO_MUX_GPIO_1_0_LINE_4 = 7,
	GPIO_MUX_GPIO_1_8_LINE_4 = 8,
	GPIO_MUX_GPIO_2_0_LINE_4 = 9,

	GPIO_MUX_GPIO_0_5_LINE_5 = 0,
	GPIO_MUX_GPIO_0_13_LINE_5 = 1,
	GPIO_MUX_GPIO_1_5_LINE_5 = 2,
	GPIO_MUX_GPIO_1_13_LINE_5 = 3,
	GPIO_MUX_GPIO_2_5_LINE_5 = 4,
	GPIO_MUX_GPIO_0_1_LINE_5 = 5,
	GPIO_MUX_GPIO_0_9_LINE_5 = 6,
	GPIO_MUX_GPIO_1_1_LINE_5 = 7,
	GPIO_MUX_GPIO_1_9_LINE_5 = 8,
	GPIO_MUX_GPIO_2_1_LINE_5 = 9,

	GPIO_MUX_GPIO_0_6_LINE_6 = 0,
	GPIO_MUX_GPIO_0_14_LINE_6 = 1,
	GPIO_MUX_GPIO_1_6_LINE_6 = 2,
	GPIO_MUX_GPIO_1_14_LINE_6 = 3,
	GPIO_MUX_GPIO_2_6_LINE_6 = 4,
	GPIO_MUX_GPIO_0_2_LINE_6 = 5,
	GPIO_MUX_GPIO_0_10_LINE_6 = 6,
	GPIO_MUX_GPIO_1_2_LINE_6 = 7,
	GPIO_MUX_GPIO_1_10_LINE_6 = 8,
	GPIO_MUX_GPIO_2_2_LINE_6 = 9,

	GPIO_MUX_GPIO_0_7_LINE_7 = 0,
	GPIO_MUX_GPIO_0_15_LINE_7 = 1,
	GPIO_MUX_GPIO_1_7_LINE_7 = 2,
	GPIO_MUX_GPIO_1_15_LINE_7 = 3,
	GPIO_MUX_GPIO_2_7_LINE_7 = 4,
	GPIO_MUX_GPIO_0_3_LINE_7 = 5,
	GPIO_MUX_GPIO_0_11_LINE_7 = 6,
	GPIO_MUX_GPIO_1_3_LINE_7 = 7,
	GPIO_MUX_GPIO_1_11_LINE_7 = 8,
	GPIO_MUX_GPIO_2_3_LINE_7 = 9,
	*/


	GPIO_MUX_LINE_0_GPIO_0_0 = 0,
	GPIO_MUX_LINE_0_GPIO_0_8 = 1,
	GPIO_MUX_LINE_0_GPIO_1_0 = 2,
	GPIO_MUX_LINE_0_GPIO_1_8 = 3,
	GPIO_MUX_LINE_0_GPIO_2_0 = 4,
	GPIO_MUX_LINE_0_GPIO_0_4 = 5,
	GPIO_MUX_LINE_0_GPIO_0_12 = 6,
	GPIO_MUX_LINE_0_GPIO_1_4 = 7,
	GPIO_MUX_LINE_0_GPIO_1_12 = 8,
	GPIO_MUX_LINE_0_GPIO_2_4 = 9,

	GPIO_MUX_LINE_1_GPIO_0_1 = 0,
	GPIO_MUX_LINE_1_GPIO_0_9 = 1,
	GPIO_MUX_LINE_1_GPIO_1_1 = 2,
	GPIO_MUX_LINE_1_GPIO_1_9 = 3,
	GPIO_MUX_LINE_1_GPIO_2_1 = 4,
	GPIO_MUX_LINE_1_GPIO_0_5 = 5,
	GPIO_MUX_LINE_1_GPIO_0_13 = 6,
	GPIO_MUX_LINE_1_GPIO_1_5 = 7,
	GPIO_MUX_LINE_1_GPIO_1_13 = 8,
	GPIO_MUX_LINE_1_GPIO_2_5 = 9,

	GPIO_MUX_LINE_2_GPIO_0_2 = 0,
	GPIO_MUX_LINE_2_GPIO_0_10 = 1,
	GPIO_MUX_LINE_2_GPIO_1_2 = 2,
	GPIO_MUX_LINE_2_GPIO_1_10 = 3,
	GPIO_MUX_LINE_2_GPIO_2_2 = 4,
	GPIO_MUX_LINE_2_GPIO_0_6 = 5,
	GPIO_MUX_LINE_2_GPIO_0_14 = 6,
	GPIO_MUX_LINE_2_GPIO_1_6 = 7,
	GPIO_MUX_LINE_2_GPIO_1_14 = 8,
	GPIO_MUX_LINE_2_GPIO_2_6 = 9,

	GPIO_MUX_LINE_3_GPIO_0_3 = 0,
	GPIO_MUX_LINE_3_GPIO_0_11 = 1,
	GPIO_MUX_LINE_3_GPIO_1_3 = 2,
	GPIO_MUX_LINE_3_GPIO_1_11 = 3,
	GPIO_MUX_LINE_3_GPIO_2_3 = 4,
	GPIO_MUX_LINE_3_GPIO_0_7 = 5,
	GPIO_MUX_LINE_3_GPIO_0_15 = 6,
	GPIO_MUX_LINE_3_GPIO_1_7 = 7,
	GPIO_MUX_LINE_3_GPIO_1_15 = 8,
	GPIO_MUX_LINE_3_GPIO_2_7 = 9,

	GPIO_MUX_LINE_4_GPIO_0_4 = 0,
	GPIO_MUX_LINE_4_GPIO_0_12 = 1,
	GPIO_MUX_LINE_4_GPIO_1_4 = 2,
	GPIO_MUX_LINE_4_GPIO_1_12 = 3,
	GPIO_MUX_LINE_4_GPIO_2_4 = 4,
	GPIO_MUX_LINE_4_GPIO_0_0 = 5,
	GPIO_MUX_LINE_4_GPIO_0_8 = 6,
	GPIO_MUX_LINE_4_GPIO_1_0 = 7,
	GPIO_MUX_LINE_4_GPIO_1_8 = 8,
	GPIO_MUX_LINE_4_GPIO_2_0 = 9,

	GPIO_MUX_LINE_5_GPIO_0_5 = 0,
	GPIO_MUX_LINE_5_GPIO_0_13 = 1,
	GPIO_MUX_LINE_5_GPIO_1_5 = 2,
	GPIO_MUX_LINE_5_GPIO_1_13 = 3,
	GPIO_MUX_LINE_5_GPIO_2_5 = 4,
	GPIO_MUX_LINE_5_GPIO_0_1 = 5,
	GPIO_MUX_LINE_5_GPIO_0_9 = 6,
	GPIO_MUX_LINE_5_GPIO_1_1 = 7,
	GPIO_MUX_LINE_5_GPIO_1_9 = 8,
	GPIO_MUX_LINE_5_GPIO_2_1 = 9,

	GPIO_MUX_LINE_6_GPIO_0_6 = 0,
	GPIO_MUX_LINE_6_GPIO_0_14 = 1,
	GPIO_MUX_LINE_6_GPIO_1_6 = 2,
	GPIO_MUX_LINE_6_GPIO_1_14 = 3,
	GPIO_MUX_LINE_6_GPIO_2_6 = 4,
	GPIO_MUX_LINE_6_GPIO_0_2 = 5,
	GPIO_MUX_LINE_6_GPIO_0_10 = 6,
	GPIO_MUX_LINE_6_GPIO_1_2 = 7,
	GPIO_MUX_LINE_6_GPIO_1_10 = 8,
	GPIO_MUX_LINE_6_GPIO_2_2 = 9,

	GPIO_MUX_LINE_7_GPIO_0_7 = 0,
	GPIO_MUX_LINE_7_GPIO_0_15 = 1,
	GPIO_MUX_LINE_7_GPIO_1_7 = 2,
	GPIO_MUX_LINE_7_GPIO_1_15 = 3,
	GPIO_MUX_LINE_7_GPIO_2_7 = 4,
	GPIO_MUX_LINE_7_GPIO_0_3 = 5,
	GPIO_MUX_LINE_7_GPIO_0_11 = 6,
	GPIO_MUX_LINE_7_GPIO_1_3 = 7,
	GPIO_MUX_LINE_7_GPIO_1_11 = 8,
	GPIO_MUX_LINE_7_GPIO_2_3 = 9,

	GPIO_MUX_LINE0 = 10
} GPIO_Line_Mux;

#define GPIO_MODE_BIT_LEVEL 1
#define GPIO_MODE_BIT_EDGE 2
#define GPIO_MODE_BIT_ANYEDGE 4
#define GPIO_MODE_BIT_INT 8

/** Режим прерывания линии GPIO
 * \note В режиме события (EVENT) функция инициализации не включает прерывания GPIO в EPIC
 */
typedef enum {
	GPIO_MODE_INT_LOW = 0b1000,    /**< Режим прерывания по низкому уровню на выводе */
	GPIO_MODE_INT_HIGH = 0b1001,   /**< Режим прерывания по высокому уровню на выводе */
	GPIO_MODE_INT_FALLING = 0b1010,/**< Режим прерывания по смене уровня на выводе с высокого на низкий */
	GPIO_MODE_INT_RISING = 0b1011, /**< Режим прерывания по смене уровня на выводе с низкого на высокий */
	GPIO_MODE_INT_CHANGE = 0b1110,  /**< Режим прерывания по изменению уровня на выводе */
	GPIO_MODE_EVENT_LOW = 0b0000,    /**< Режим события по низкому уровню на выводе */
	GPIO_MODE_EVENT_HIGH = 0b0001,   /**< Режим события по высокому уровню на выводе */
	GPIO_MODE_EVENT_FALLING = 0b0010,/**< Режим события по смене уровня на выводе с высокого на низкий */
	GPIO_MODE_EVENT_RISING = 0b0011, /**< Режим события по смене уровня на выводе с низкого на высокий */
	GPIO_MODE_EVENT_CHANGE = 0b0110  /**< Режим события по изменению уровня на выводе */
} GPIO_InterruptMode;

/** Функция инициализации линии прерывания
 *
 * \param irq_line Номер линии прерывания
 * \param mux Настройка мультиплексора линии прерывания
 * \param mode Режим линии прерывания
 * \note Номер линии прерывания можно получить после настройки мультиплексора.
 * Введите в mux GPIO_MUX_GPIO_X_X, где X - номера порта и вывода,
 * и нажмите Ctrl+Пробел: появятся варианты констант для данного вывода,
 * далее достаточно выбрать константу для доступной линии
 */
void GPIO_InitInterruptLine(GPIO_Line irq_line, GPIO_Line_Mux mux,
		GPIO_InterruptMode mode);


/** Функция деинициализации линии прерывания, выключает линию и сбрасывает её настройки
 *
 * \param irq_line Номер линии прерывания
 */
void GPIO_DeInitInterruptLine(GPIO_Line irq_line);


/** Функция получения состояния линии прерывания
 *
 * \param irq_line Номер линии прерывания
 * \return Возвращает true если сработало прерывание данной линии
 */
bool GPIO_LineInterruptState(GPIO_Line irq_line);

#define GPIO_LINE_INTERRUPT_STATE(irq_line) ((GPIO_IRQ->INTERRUPTS & (1 << (irq_line))) != 0)

/** Функция чтения логического уровня вывода, подключенного к линии прерывания
 *
 * \param irq_line Номер линии прерывания
 * \return Логический уровень вывода
 */
GPIO_PinState GPIO_LinePinState(GPIO_Line irq_line);

#define GPIO_LINE_PIN_STATE(irq_line) (GPIO_IRQ->STATE & (1 << (irq_line)))

/** Функция сброса регистра состояния прерываний.
 *  \note Когда срабатывает прерывание на одной из линии, в регистре INTERRUPT
 *  выставляется 1 в разряде, соответствующем линии прерывания.
 *  После обработки прерывания необходимо сбросить данный регистр
 *  в обработчике прерывания GPIO_IRQ_TRAP_HANDLER().
 *  Если после обработки прерывания регистр не был сброшен,
 *  обработчик будет вызван снова, программа будет бесконечно вызывать обработчик.
 */
void GPIO_ClearInterrupt();

#define GPIO_CLEAR_INTERRUPT()  (GPIO_IRQ->CLEAR = 0b11111111)

/** Функция включения прерываний ввода-вывода
 *  \note Функция включает данный вид прерываний в EPIC
 */
void GPIO_EnableInterrupts();

/** Функция отключения прерываний ввода-вывода
*   \note Функция включает данный вид прерываний в EPIC
*/
void GPIO_DisableInterrupts();


#endif
