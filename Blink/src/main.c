#include <mcu32_memory_map.h>
#include <pad_config.h>
#include <gpio.h>
#include <power_manager.h>
#include <wakeup.h>

/*
* Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
* В примере настраивается вывод, который подключенный к светодиоду, в режим GPIO. 
* Если выбрана плата с пользовательской кнопкой (BOARD_LITE), то становится доступна функция ledButton, которая считывает
* состояние кнопки и зажигает светодиод.
*
* Плата выбирается ниже в #define 
* В зависимости от версии контроллера MIK32V0 или MIK32V2 выводы настраиваются в режим GPIO по-разному. 
*/


/* Тип платы */
#define BOARD_LITE
// #define BOARD_DIP


#ifdef BOARD_LITE
#define PIN_LED 7	 // Светодиод управляется выводом PORT_2_7
#define PIN_BUTTON 6 // Кнопка управляет сигналом на выводе PORT_2_6
#endif

#ifdef BOARD_DIP
#define PIN_LED1 3	 // Светодиод управляется выводом PORT_0_3
#define PIN_LED2 3	 // Светодиод управляется выводом PORT_1_3
#endif

void InitClock()
{
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_UART_0_M | PM_CLOCK_APB_P_GPIO_0_M | PM_CLOCK_APB_P_GPIO_1_M | PM_CLOCK_APB_P_GPIO_2_M; // включение тактирования GPIO
	PM->CLK_APB_M_SET |= PM_CLOCK_APB_M_PAD_CONFIG_M | PM_CLOCK_APB_M_WU_M | PM_CLOCK_APB_M_PM_M;								// включение тактирования блока для смены режима выводов
}

void ledBlink()
{
#ifdef BOARD_LITE
	GPIO_2->OUTPUT ^= 1 << PIN_LED; // Установка сигнала вывода 7 порта 2 в противоположный уровень
	for (volatile int i = 0; i < 100000; i++)
		;
#endif

#ifdef BOARD_DIP
	GPIO_0->OUTPUT ^= 1 << PIN_LED1; // Установка сигнала вывода 3 порта 0 в противоположный уровень
	GPIO_1->OUTPUT ^= 1 << PIN_LED2; // Установка сигнала вывода 3 порта 1 в противоположный уровень
	for (volatile int i = 0; i < 100000; i++)
		;
#endif
}

#ifdef BOARD_LITE
void ledButton()
{
	if (GPIO_2->SET == (1 << PIN_BUTTON))
	{
		GPIO_2->OUTPUT |= 1 << PIN_LED; // Установка сигнала вывода 7 порта 2 в высокий уровень
	}
	else
	{
		GPIO_2->OUTPUT &= ~(1 << PIN_LED); // Установка сигнала вывода 7 порта в низкий уровень
	}
}
#endif

int main()
{
	InitClock(); // Включение тактирования GPIO

#ifdef BOARD_LITE
#ifdef MIK32V0
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_LED));	   // Установка вывода 7 порта 2 в режим GPIO
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_BUTTON)); // Установка вывода 6 порта 2 в режим GPIO

	GPIO_2->DIRECTION_OUT = 1 << PIN_LED;	// Установка направления вывода 7 порта 2 на выход
	GPIO_2->DIRECTION_IN = 1 << PIN_BUTTON; // Установка направления вывода 6 порта 2 на вход
#endif

#ifdef MIK32V2
	PAD_CONFIG->PORT_2_CFG &= ~(0b11 << (2 * PIN_LED));	// Установка вывода 7 порта 2 в режим GPIO
	PAD_CONFIG->PORT_2_CFG &= ~(0b11 << (2 * PIN_BUTTON)); // Установка вывода 6 порта 2 в режим GPIO

	GPIO_2->DIRECTION_OUT = 1 << PIN_LED;	// Установка направления вывода 7 порта 2 на выход
	GPIO_2->DIRECTION_IN = 1 << PIN_BUTTON; // Установка направления вывода 6 порта 2 на вход
#endif
#endif


#ifdef BOARD_DIP
#ifdef MIK32V0
	PAD_CONFIG->PORT_0_CFG |= (1 << (2 * PIN_LED1));	// Установка вывода 3 порта 0 в режим GPIO
	PAD_CONFIG->PORT_1_CFG |= (1 << (2 * PIN_LED2)); 	// Установка вывода 3 порта 1 в режим GPIO

	GPIO_0->DIRECTION_OUT = 1 << PIN_LED1;	// Установка направления вывода 3 порта 0 на выход
	GPIO_1->DIRECTION_OUT = 1 << PIN_LED2;	// Установка направления вывода 3 порта 1 на выход

#endif

#ifdef MIK32V2
	PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * PIN_LED1));	// Установка вывода 3 порта 0 в режим GPIO
	PAD_CONFIG->PORT_1_CFG &= ~(0b11 << (2 * PIN_LED2)); 	// Установка вывода 3 порта 1 в режим GPIO

	GPIO_0->DIRECTION_OUT = 1 << PIN_LED1;	// Установка направления вывода 3 порта 0 на выход
	GPIO_1->DIRECTION_OUT = 1 << PIN_LED2;	// Установка направления вывода 3 порта 1 на выход
#endif
#endif

	while (1)
	{
		ledBlink(); /* Светодиод мигает */

#ifdef BOARD_LITE
		// ledButton(); /* Светодиод зажигается при нажатой кнопке */
#endif
	}
}