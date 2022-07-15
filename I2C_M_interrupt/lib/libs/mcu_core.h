#ifndef FPGA_MCU_CORE_H_INCLUDED
#define FPGA_MCU_CORE_H_INCLUDED

/**
 * \file Библиотека для работы с ядром и основными функциями МК
 */

#include <stdint.h>
#include <stdbool.h>

#include "csr.h"
#include "scr1_csr_encoding.h"

#include "mcu32_memory_map.h"
#include "pad_config.h"
#include "epic.h"
#include "gpio.h"
#include "gpio_irq.h"

#ifndef FREQ
#define FREQ 10000000
#endif


__attribute__ ((weak)) void TIMER32_0_TRAP_HANDLER();
__attribute__ ((weak)) void UART_0_TRAP_HANDLER();
__attribute__ ((weak)) void UART_1_TRAP_HANDLER();
__attribute__ ((weak)) void SPI_0_TRAP_HANDLER();
__attribute__ ((weak)) void SPI_1_TRAP_HANDLER();
__attribute__ ((weak)) void GPIO_IRQ_TRAP_HANDLER();
__attribute__ ((weak)) void I2C_0_TRAP_HANDLER();
__attribute__ ((weak)) void I2C_1_TRAP_HANDLER();
__attribute__ ((weak)) void WDT_TRAP_HANDLER();
__attribute__ ((weak)) void TIMER16_0_TRAP_HANDLER();
__attribute__ ((weak)) void TIMER16_1_TRAP_HANDLER();
__attribute__ ((weak)) void TIMER16_2_TRAP_HANDLER();
__attribute__ ((weak)) void TIMER32_1_TRAP_HANDLER();
__attribute__ ((weak)) void TIMER32_2_TRAP_HANDLER();
__attribute__ ((weak)) void EEPROM_TRAP_HANDLER();
__attribute__ ((weak)) void SPIFI_TRAP_HANDLER();
__attribute__ ((weak)) void RTC_TRAP_HANDLER();
__attribute__ ((weak)) void WDT_DOM3_TRAP_HANDLER();
__attribute__ ((weak)) void WDT_SPIFI_TRAP_HANDLER();
__attribute__ ((weak)) void WDT_EEPROM_TRAP_HANDLER();
__attribute__ ((weak)) void DMA_GLB_ERR_TRAP_HANDLER();
__attribute__ ((weak)) void DMA_CHANNELS_TRAP_HANDLER();
__attribute__ ((weak)) void FREQ_MON_TRAP_HANDLER();
__attribute__ ((weak)) void PVD_AVCC_TRAP_UNDER_HANDLER();
__attribute__ ((weak)) void PVD_AVCC_TRAP_OVER_HANDLER();
__attribute__ ((weak)) void PVD_VCC_TRAP_UNDER_HANDLER();
__attribute__ ((weak)) void PVD_VCC_TRAP_OVER_HANDLER();
__attribute__ ((weak)) void BATTERY_NON_GOOD_TRAP_HANDLER();
__attribute__ ((weak)) void BOR_TRAP_HANDLER();
__attribute__ ((weak)) void TSENS_TRAP_HANDLER();
__attribute__ ((weak)) void ADC_TRAP_HANDLER();
__attribute__ ((weak)) void PROG_TRAP_HANDLER();


/**
 *
 */
typedef enum {
	PIN_FUNCTION_0 = 0,    /**< Функция вывода 0 */
	PIN_FUNCTION_1 = 1,    /**< Функция вывода 1 */
	PIN_FUNCTION_2 = 2,    /**< Функция вывода 2 */
	PIN_FUNCTION_MAIN = 0, /**< Основная функция вывода */
	PIN_FUNCTION_GPIO = 1, /**< Режим ввода-вывода */
	PIN_FUNCTION_ALT = 2   /**< Дополнительная функция вывода */
} PadConfigFunction;

typedef enum {
	PIN_0 = (1 << 0),
	PIN_1 = (1 << 1),
	PIN_2 = (1 << 2),
	PIN_3 = (1 << 3),
	PIN_4 = (1 << 4),
	PIN_5 = (1 << 5),
	PIN_6 = (1 << 6),
	PIN_7 = (1 << 7),
	PIN_8 = (1 << 8),
	PIN_9 = (1 << 9),
	PIN_10 = (1 << 10),
	PIN_11 = (1 << 11),
	PIN_12 = (1 << 12),
	PIN_13 = (1 << 13),
	PIN_14 = (1 << 14),
	PIN_15 = (1 << 15),
} PinId;


/** Устанавливает функцию отдельного вывода
 *
 * \param port Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param pinId Номер вывода, принимает значения 0..15 для портов 0..1 и значения 0..7 для порта 2
 * \param func Функция вывода
 * \return В случае неверных входных параметров возвращает false
 */
bool setPinFunction(GPIO_TypeDef *port, uint16_t pinId, PadConfigFunction func);

/** Устанавливает функцию выводов по маске
 *
 * @param port Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * @param mask Маска выводов
 * @param func Функция вывода
 * @return
 */
bool setPinMaskFunction(GPIO_TypeDef *port, uint16_t mask, PadConfigFunction func);

/** Устанавливает функцию всех выводов порта
 *
 * \param port Порт ввода-вывода, принимает значения констант GPIO_0, GPIO_1, GPIO_2
 * \param func Функция вывода
 * \return В случае неверных входных параметров возвращает false
 */
bool setPortFunction(GPIO_TypeDef *port, PadConfigFunction func);


/**	Функция включения прерываний в ядре МК
 * \note Функция управляет регистрами mstatus и mie risc-v ядра, регистры EPIC не изменяются
 */
void enableInterrupts();

/**	Функция отключения прерываний в ядре МК
 * \note Функция управляет регистрами mstatus и mie risc-v ядра, регистры EPIC не изменяются
 */
void disableInterrupts();


#endif /* FPGA_MCU_CORE_H_INCLUDED */
