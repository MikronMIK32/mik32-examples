#include "mik32_hal_irq.h"


void HAL_IRQ_EnableInterrupts()
{   
    set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}

void HAL_IRQ_DisableInterrupts()
{    
    clear_csr(mie, MIE_MEIE);
}

void HAL_EPIC_MaskLevelSet(uint32_t Interrupt_line)
{
    EPIC->MASK_LEVEL_SET = 1 << Interrupt_line; // Прерывание по уровню
}

void HAL_EPIC_MaskLevelClear(uint32_t Interrupt_line)
{
    EPIC->MASK_LEVEL_CLEAR |= 1 << Interrupt_line; // Прерывание по уровню
}