#ifndef MIK32_HAL_IRQ
#define MIK32_HAL_IRQ

#include "mcu32_memory_map.h"
#include "epic.h"
#include "csr.h"
#include "scr1_csr_encoding.h"

void HAL_IRQ_EnableInterrupts();
void HAL_IRQ_DisableInterrupts();
/* Прерывание по уровню */
void HAL_EPIC_MaskLevelSet(uint32_t Interrupt_line);
void HAL_EPIC_MaskLevelClear(uint32_t Interrupt_line);


#endif