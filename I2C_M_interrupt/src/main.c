#include "common.h"
// #include "i2c_irq_common.h"
#include "mcu_core.h"
#include "i2c.h"
#include "epic.h"

I2C_TypeDef* i2c;
volatile uint32_t epic_index;
uint32_t trap_handler_called = 0;

// void I2C_0_TRAP_HANDLER()
// {
//     xprintf("\ntrap_handler()\n");
    
//     uint32_t epic_mask;
//     trap_handler_called++;
//     CheckTrapHandlerCalledOnce(trap_handler_called);
//     epic_index = 6;
//     epic_mask = (1 << epic_index);
//     xprintf("epic_index = %d\n", epic_index);
//     xprintf("epic_mask = %d\n", epic_mask);
//     epic_mask = (1 << 6);
//     if ((EPIC->RAW_STATUS & epic_mask) == 0)
//     {
//         TEST_ERROR("EIC source [%d] is zero unexpectedly", epic_index);
//     } 
//     i2c->ICR = 0xFFFFFFFF;
//     if ((EPIC->RAW_STATUS & epic_mask) != 0)
//     {
//         TEST_ERROR("EPIC source [%d] has not been cleared", epic_index);
//     } 
//     EPIC->CLEAR = epic_mask;    // clear flag in EPIC
// }

// __attribute__((interrupt))
void trap_handler() 
{
    xprintf("\ntrap_handler()\n");
    xprintf("EIC source [%d] is zero unexpectedly", trap_handler_called);
    uint32_t epic_mask;
    trap_handler_called++;
    CheckTrapHandlerCalledOnce(trap_handler_called);
    //epic_index = 6;
    epic_mask = (1 << epic_index);
    xprintf("epic_index = %d\n", epic_index);
    xprintf("epic_mask = %d\n", epic_mask);
    epic_mask = (1 << 6);
    if ((EPIC->RAW_STATUS & epic_mask) == 0)
    {
        TEST_ERROR("EIC source [%d] is zero unexpectedly", epic_index);
    } 
    i2c->ICR = 0xFFFFFFFF;
    if ((EPIC->RAW_STATUS & epic_mask) != 0)
    {
        TEST_ERROR("EPIC source [%d] has not been cleared", epic_index);
    } 
    EPIC->CLEAR = epic_mask;    // clear flag in EPIC
    EPIC->MASK_LEVEL_CLEAR = 0xFFFFFFFF;
}


void Init(I2C_TypeDef* i2c)
{
    i2c->CR1 = 0;

    i2c->TIMINGR = I2C_TIMINGR_SCLDEL(1) | I2C_TIMINGR_SDADEL(1) |
    I2C_TIMINGR_SCLL(20) | I2C_TIMINGR_SCLH(20) | I2C_TIMINGR_PRESC(3); //частота 164,7 кГц tsync1 + tsync2 = 10^(-6)

    i2c->CR1 = I2C_CR1_PE_M | I2C_CR1_STOPIE_M;// | I2C_CR1_TCIE_M | I2C_CR1_ADDRIE_M | I2C_CR1_NACKIE_M | I2C_CR1_ERRIE_M | I2C_CR1_RXIE_M | I2C_CR1_TXIE_M;
}

void Wait(I2C_TypeDef* i2c)
{
    for(volatile int i = 0; i < 100; i++)
    {
        if (i2c->ISR & I2C_ISR_STOPF_M)
        {
            break;
        }
    }
}

int main()
{

    EnableAllClocks();
    
    for (volatile int i = 0; i < 10; i++);

    i2c = I2C_0;
    epic_index = 6;
    uint32_t epic_mask = 1 << epic_index;
    EPIC->MASK_SET = 0xFFFFFFFF; 
    EPIC->MASK_LEVEL_SET = 0xFFFFFFFF;
    EnableInterrupts();
    
    //xprintf("Activate I2C interrupt\n");    
    
    Init(i2c);

    while (1)
    {
    
        /*
        *
        *   uint32_t epic_mask;
        *   i2c = _i2c;
        *   epic_index = _epic_index;
        *   EPIC->MASK_SET = 0xFFFFFFFF;
        * 
        */
        
            // PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_I2C_0_M;
    // PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_EPIC_M | PM_CLOCK_PM_M ;
    
    //EnableInterrupts();
    xprintf("EIC source [%d] is zero unexpectedly", trap_handler_called);
    xprintf("Activate I2C interrupt\n"); 
    trap_handler_called = 0;
    xprintf("Test1 = %d\n", trap_handler_called);
    trap_handler_called = 1;
    xprintf("Test2 = %d\n", trap_handler_called); 
    /*
    *
    * CR2 - регистр управления 2
    * 
    * SADD - адрес ведомого
    * 
    * RD_WRN - Направление передачи: 0 – ведущий в режиме записи; 1 – ведущий в режиме чтения
    * 
    * Количество байт для приема / передачи
    * 
    * AUTOEND - Управление режимом автоматического окончания: 0 – автоматическое окончание выкл; 1 – автоматическе окончание вкл
    * 
    */
    i2c->CR2 = I2C_CR2_SADD(0x36) | I2C_CR2_WR_M | I2C_CR2_NBYTES(0) | I2C_CR2_AUTOEND_M; 
    i2c->CR2 |= I2C_CR2_START_M; // старт отправки адреса, а затем данных 

    Wait(i2c);
    xprintf("Stopf = %d, stopie = %d\n", (i2c->ISR & I2C_ISR_STOPF_M)>> I2C_ISR_STOPF_S, (i2c->CR1 & I2C_CR1_STOPIE_M)>> I2C_CR1_STOPIE_S);
    //CheckTrapHandlerCalledOnce(trap_handler_called);
    for (volatile int i = 0; i < 1000000; i++); 
    //i2c->ICR = 0xFFFFFFFF;
        
    }
    
}