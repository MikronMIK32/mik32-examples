#include <mcu32_memory_map.h>
#include <pad_config.h>
#include <gpio.h>
#include <power_manager.h>
#include "common.h"

#define PIN_LED2 7 // LED2 управляется выводом PORT_2_7
#define PIN_button 6 // LED2 управляется выводом PORT_2_6

void initClock() {
	PM->CLK_APB_P_SET |=  PM_CLOCK_GPIO_2_M; // включение тактирования GPIO2
	PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M; // включение тактирования блока для смены режима выводов
	for (volatile int i = 0; i < 10; i++) ;
}

void ledBlink() {
	GPIO_2->OUTPUT |= 1 << PIN_LED2;   //Установка значения вывода 7 порта 2 в высокий уровень
	xprintf("вкл \n");
	for (volatile int i = 0; i < 1000000; i++);
	GPIO_2->OUTPUT &= ~(1 << PIN_LED2); //Установка значения вывода 7 порта  в низкий уровень   
	xprintf("выкл \n");
	for (volatile int i = 0; i < 1000000; i++);
}

void ledButton() {
	if(GPIO_2->SET == (1 << PIN_button))
	{
		GPIO_2->OUTPUT |= 1 << PIN_LED2;   //Установка значения вывода 7 порта 2 в высокий уровень
	}
	else
	{
		GPIO_2->OUTPUT &= ~(1 << PIN_LED2); //Установка значения вывода 7 порта  в низкий уровень
	}
}

void main() {
	initClock(); //включние тактирования GPIO2

	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_LED2)); // Установка порта 2 в режим GPIO
	PAD_CONFIG->PORT_2_CFG |= (1 << (2 * PIN_button)); // Установка порта 2 в режим GPIO

	GPIO_2->DIRECTION_OUT = 1 << PIN_LED2; // Установка направления вывода  в выход
	GPIO_2->DIRECTION_IN = 1 << PIN_button; // Установка направления порта  в вход

	while (1) {
		//ledBlink();
		ledButton();
	}
}