#include <mcu32_memory_map.h>
#include <pad_config.h>
#include <gpio.h>
#include <power_manager.h>

/* Тип платы */
#define BOARD_LITE_V0

#ifdef BOARD_LITE_V0
#define PIN_LED 	7 // Светодиод управляется выводом PORT_2_7
#define PIN_BUTTON 	6 // Кнопка управляет сигналом на выводе PORT_2_6
#endif 

void InitClock() 
{
	PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_2_M; // включение тактирования GPIO
	PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M; // включение тактирования блока для смены режима выводов
}

void ledBlink() 
{
	#ifdef BOARD_LITE_V0
	GPIO_2->OUTPUT |= 1 << PIN_LED;   // Установка сигнала вывода 7 порта 2 в высокий уровень
	for (volatile int i = 0; i < 100000; i++);
	GPIO_2->OUTPUT &= ~(1 << PIN_LED); // Установка сигнала вывода 7 порта  в низкий уровень   
	for (volatile int i = 0; i < 100000; i++);
	#endif 
}

void ledButton() 
{
	#ifdef BOARD_LITE_V0
	if(GPIO_2->SET == (1 << PIN_BUTTON))
	{
		GPIO_2->OUTPUT |= 1 << PIN_LED;   // Установка сигнала вывода 7 порта 2 в высокий уровень
	}
	else
	{
		GPIO_2->OUTPUT &= ~(1 << PIN_LED); // Установка сигнала вывода 7 порта в низкий уровень
	}
	#endif 
}

int main() 
{
	InitClock(); // Включение тактирования GPIO

	#ifdef BOARD_LITE_V0
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_LED)); 	// Установка вывода 7 порта 2 в режим GPIO
	GPIO_2->DIRECTION_OUT = 1 << PIN_LED; 				// Установка направления вывода 7 порта 2 на выход
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_BUTTON)); 	// Установка вывода 6 порта 2 в режим GPIO
	GPIO_2->DIRECTION_IN = 1 << PIN_BUTTON; 			// Установка направления вывода 6 порта 2 на вход
	#endif 

	while (1) 
	{
		ledBlink(); /* Светодиод мигает */
		// ledButton(); /* Светодиод зажигается при нажатой кнопке */ 
	}
}