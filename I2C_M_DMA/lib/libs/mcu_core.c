#include "mcu_core.h"

__attribute__ ((weak)) void TIMER32_0_TRAP_HANDLER() {}
__attribute__ ((weak)) void UART_0_TRAP_HANDLER() {}
__attribute__ ((weak)) void UART_1_TRAP_HANDLER() {}
__attribute__ ((weak)) void SPI_0_TRAP_HANDLER() {}
__attribute__ ((weak)) void SPI_1_TRAP_HANDLER() {}
__attribute__ ((weak)) void GPIO_IRQ_TRAP_HANDLER() {}
__attribute__ ((weak)) void I2C_0_TRAP_HANDLER() {}
__attribute__ ((weak)) void I2C_1_TRAP_HANDLER() {}
__attribute__ ((weak)) void WDT_TRAP_HANDLER() {}
__attribute__ ((weak)) void TIMER16_0_TRAP_HANDLER() {}
__attribute__ ((weak)) void TIMER16_1_TRAP_HANDLER() {}
__attribute__ ((weak)) void TIMER16_2_TRAP_HANDLER() {}
__attribute__ ((weak)) void TIMER32_1_TRAP_HANDLER() {}
__attribute__ ((weak)) void TIMER32_2_TRAP_HANDLER() {}
__attribute__ ((weak)) void EEPROM_TRAP_HANDLER() {}
__attribute__ ((weak)) void SPIFI_TRAP_HANDLER() {}
__attribute__ ((weak)) void RTC_TRAP_HANDLER() {}
__attribute__ ((weak)) void WDT_DOM3_TRAP_HANDLER() {}
__attribute__ ((weak)) void WDT_SPIFI_TRAP_HANDLER() {}
__attribute__ ((weak)) void WDT_EEPROM_TRAP_HANDLER() {}
__attribute__ ((weak)) void DMA_GLB_ERR_TRAP_HANDLER() {}
__attribute__ ((weak)) void DMA_CHANNELS_TRAP_HANDLER() {}
__attribute__ ((weak)) void FREQ_MON_TRAP_HANDLER() {}
__attribute__ ((weak)) void PVD_AVCC_TRAP_UNDER_HANDLER() {}
__attribute__ ((weak)) void PVD_AVCC_TRAP_OVER_HANDLER() {}
__attribute__ ((weak)) void PVD_VCC_TRAP_UNDER_HANDLER() {}
__attribute__ ((weak)) void PVD_VCC_TRAP_OVER_HANDLER() {}
__attribute__ ((weak)) void BATTERY_NON_GOOD_TRAP_HANDLER() {}
__attribute__ ((weak)) void BOR_TRAP_HANDLER() {}
__attribute__ ((weak)) void TSENS_TRAP_HANDLER() {}
__attribute__ ((weak)) void ADC_TRAP_HANDLER() {}
__attribute__ ((weak)) void PROG_TRAP_HANDLER() {}

void trap_handler() {
	if (EPIC->STATUS & (1 << EPIC_TIMER32_0_INDEX)) {
		TIMER32_0_TRAP_HANDLER();
	}
	if (EPIC->STATUS & (1 << EPIC_GPIO_IRQ_INDEX)) {
		GPIO_IRQ_TRAP_HANDLER();
	}
	if (EPIC->STATUS & (1 << EPIC_TIMER32_1_INDEX)) {
		TIMER32_1_TRAP_HANDLER();
	}
	EPIC->CLEAR = 0xFF;
}


bool setPinFunction(GPIO_TypeDef *port, uint16_t pinId, PadConfigFunction func) {
	if (func > 2) {
		return false;
	}

	if (pinId > 31) {
		return false;
	}
	if (port == GPIO_0) {
		PAD_CONFIG->PORT_0_CFG &= ~(3 << (pinId << 1));
		PAD_CONFIG->PORT_0_CFG |= func << (pinId << 1);

		return true;
	}
	if (port == GPIO_1) {
		PAD_CONFIG->PORT_1_CFG &= ~(3 << (pinId << 1));
		PAD_CONFIG->PORT_1_CFG |= func << (pinId << 1);

		return true;
	}

	if (pinId > 15) {
		return false;
	}
	if (port == GPIO_2) {
		PAD_CONFIG->PORT_2_CFG &= ~(3 << (pinId << 1));
		PAD_CONFIG->PORT_2_CFG |= func << (pinId << 1);

		return true;
	}

	return false; // Error
}


bool setPinMaskFunction(GPIO_TypeDef *port, uint16_t mask, PadConfigFunction func) {
	if (func > 2) {
		return false;
	}

	for (int i = 0; i < (port == GPIO_2 ? 8 : 16); i++) {
		if (mask & (1 < i)) {
			if (port == GPIO_0) {
				PAD_CONFIG->PORT_0_CFG &= ~(3 << (i << 1));
				PAD_CONFIG->PORT_0_CFG |= func << (i << 1);

				return true;
			}
			if (port == GPIO_1) {
				PAD_CONFIG->PORT_1_CFG &= ~(3 << (i << 1));
				PAD_CONFIG->PORT_1_CFG |= func << (i << 1);

				return true;
			}
			if (port == GPIO_2) {
				PAD_CONFIG->PORT_2_CFG &= ~(3 << (i << 1));
				PAD_CONFIG->PORT_2_CFG |= func << (i << 1);

				return true;
			}
		}
	}

	return false; // Error
}


bool setPortFunction(GPIO_TypeDef *port, PadConfigFunction func) {
	if (func > 2) {
		return false;
	}

	if (port == GPIO_0) {
		PAD_CONFIG->PORT_0_CFG = 0x55555555 * func;

		return true;
	}
	if (port == GPIO_1) {
		PAD_CONFIG->PORT_1_CFG = 0x55555555 * func;

		return true;
	}
	if (port == GPIO_2) {
		PAD_CONFIG->PORT_2_CFG = 0x55555555 * func;

		return true;
	}

	return false; // Error
}


void enableInterrupts()
{
    set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}


void disableInterrupts()
{
    clear_csr(mie, MIE_MEIE);
}


