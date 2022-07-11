#include "mcu_digital.h"


GPIO_PinState GPIO_PinRead(GPIO_TypeDef *gpio, uint32_t gpioNum) {
	return (gpio->SET & (1 << gpioNum));
	/*
	if (gpio->SET & (1 << gpioNum)) {
		return GPIO_HIGH;
	} else {
		return GPIO_LOW;
	} */
}


void GPIO_PinWrite(GPIO_TypeDef *gpio, uint32_t gpioNum, GPIO_PinState state) {
	if (state) {
		gpio->SET = 1 << gpioNum;
	} else {
		gpio->CLEAR = 1 << gpioNum;
	}
}


void GPIO_PinToggle(GPIO_TypeDef *gpio, uint32_t gpioNum) {
	gpio->OUTPUT ^= 1 << gpioNum;
}


void GPIO_SetPinDirection(GPIO_TypeDef *gpio, uint32_t gpioNum, GPIO_PinDirection dir) {

	if (dir == GPIO_DIR_OUTPUT) {
		gpio->DIRECTION_OUT = 1 << gpioNum;
	}
	if (dir == GPIO_DIR_INPUT) {
		gpio->DIRECTION_IN = 1 << gpioNum;
	}
}

void GPIO_SetPinMaskDirection(GPIO_TypeDef *gpio, uint16_t mask, GPIO_PinDirection dir) {
	if (dir == GPIO_DIR_OUTPUT) {
		gpio->DIRECTION_OUT = mask;
	}
	if (dir == GPIO_DIR_INPUT) {
		gpio->DIRECTION_IN = mask;
	}
}

/** Для обхода бага МК, чтение из регистра IRQ_LINE_MUX всегда возвращает 0
 *  \note Используется в функциях GPIO_InitInterruptLine и GPIO_DeInitInterruptLine
 */
uint32_t current_irq_line_mux = 0;

void GPIO_InitInterruptLine(GPIO_Line irq_line, GPIO_Line_Mux mux,
	GPIO_InterruptMode mode) {
	if (irq_line > 7) return;

	current_irq_line_mux &= ~(0b1111 << (irq_line << 2));
	current_irq_line_mux |= (mux << (irq_line << 2));
	GPIO_IRQ->CFG = current_irq_line_mux;

	if (mode & GPIO_MODE_BIT_LEVEL) {
		GPIO_IRQ->LEVEL_SET = (1 << irq_line);
	} else {
		GPIO_IRQ->LEVEL_CLEAR = (1 << irq_line);
	}

	if (mode & GPIO_MODE_BIT_EDGE) {
		GPIO_IRQ->EDGE |= (1 << irq_line);
	} else {
		GPIO_IRQ->EDGE &= ~(1 << irq_line);
	}

	if (mode & GPIO_MODE_BIT_ANYEDGE) {
		GPIO_IRQ->ANYEDGE_SET = (1 << irq_line);
	} else {
		GPIO_IRQ->ANYEDGE_CLEAR = (1 << irq_line);
	}

	GPIO_IRQ->ENABLE_SET = (1 << irq_line);
	if (mode & GPIO_MODE_BIT_INT) {
		EPIC->MASK_SET = 1 << EPIC_GPIO_IRQ_INDEX;
	}
}


void GPIO_DeInitInterruptLine(GPIO_Line irq_line) {
	if (irq_line > 7) return;

	current_irq_line_mux &= ~(0b1111 << (irq_line << 2));
	GPIO_IRQ->CFG = current_irq_line_mux;

	GPIO_IRQ->EDGE &= ~(1 << irq_line);
	GPIO_IRQ->LEVEL_SET &= ~(1 << irq_line);
	GPIO_IRQ->ANYEDGE_SET &= ~(1 << irq_line);

	GPIO_IRQ->ENABLE_SET &= ~(1 << irq_line);

	if (current_irq_line_mux == 0) {
		EPIC->MASK_CLEAR = 1 << EPIC_GPIO_IRQ_INDEX;
	}
}

bool GPIO_LineInterruptState(GPIO_Line irq_line) {
	return (GPIO_IRQ->INTERRUPTS & (1 << (irq_line))) != 0;
}

GPIO_PinState GPIO_LinePinState(GPIO_Line irq_line) {
	return GPIO_IRQ->STATE & (1 << (irq_line));
}

void GPIO_ClearInterrupt() {
	GPIO_IRQ->CLEAR = 0b11111111;
}

void GPIO_EnableInterrupts() {
	EPIC->MASK_SET |= 1 << EPIC_GPIO_IRQ_INDEX;
}

void GPIO_DisableInterrupts() {
	EPIC->MASK_SET |= 1 << EPIC_GPIO_IRQ_INDEX;
}

