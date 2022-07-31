
#include "common.h"
#include "wakeup.h"
#include "csr.h"
#include "scr1_csr_encoding.h"
#include "uart_lib.h"

#ifndef DEBUG_UART_DIVIDER
    // 115200 baud @ 32 MHz
    #define DEBUG_UART_DIVIDER 278
#endif

#ifndef DEBUG_UART
    #define DEBUG_UART UART_0
#endif

static inline __attribute__((always_inline)) uint32_t __Get_PM_CLOCK_DEBUG_UART(UART_TypeDef * const uart) {
    switch((uint32_t)uart) {
        case (uint32_t)UART_0: return PM_CLOCK_UART_0_M;
        case (uint32_t)UART_1: return PM_CLOCK_UART_1_M;
        //case (uint32_t)UART_2: return PM_CLOCK_UART_2_M;
        //default: return 0;
    }
}

#define __COMMON_H_CONCAT3(b) PM_CLOCK_ ## b ## _M
//#define _COMMON_H_CONCAT(Z) __COMMON_H_CONCAT3(Z)
//#define PM_CLOCK_DEBUG_UART(...) PM_CLOCK_ ## __VA_ARGS__ ## _M
//#define PM_CLOCK_DEBUG_UART(...) __COMMON_H_CONCAT3(__VA_ARGS__)
#define PM_CLOCK_DEBUG_UART __Get_PM_CLOCK_DEBUG_UART(DEBUG_UART)

volatile uint32_t* EdgeCounter_p;
volatile uint32_t* Dac0Monitor_p;
volatile uint32_t* Dac1Monitor_p;
volatile uint32_t* PeriodMeasurer_p;
volatile uint32_t* RealValueGenerator_p;
volatile uint32_t* PowerGenerator_p;

void xputc (char c) 
{

    while (!UART_IsTxBufferFreed(DEBUG_UART)) {};
    UART_WriteByte(DEBUG_UART,c);
    //UART_WaitTransmission(DEBUG_UART);

}

void WaitTransmissionEnd(void)
{
    UART_WaitTransmission(DEBUG_UART);
}

void SystemInit()
{
    WU->DOM0_MASK = 1<< WU_MASK_SYS_RESET_LDO_S | 1<< WU_MASK_SYS_RESET_VOLTMON_S | 1<< WU_MASK_SYS_RESET_BOR_S | 1<<WU_MASK_BU_RESET_BOR_S;  
    uint32_t pm_clock_debug_uart = 0;
    switch((int)DEBUG_UART) {
        case (int)UART_0: pm_clock_debug_uart = PM_CLOCK_UART_0_M; break;
        case (int)UART_1: pm_clock_debug_uart = PM_CLOCK_UART_1_M; break;
        //case (uint32_t)UART_2: return PM_CLOCK_UART_2_M;
        //default: return 0;
    }
    PM->CLK_APB_P_SET = pm_clock_debug_uart;

    UART_Init(DEBUG_UART, DEBUG_UART_DIVIDER, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    UART_WriteByte(DEBUG_UART,'\n');

    //WU->CLOCKS_BU  =50<< WU_CLOCKS_BU_ADJ_LSI32K_S  | 1 << WU_CLOCKS_BU_LSI32K_PD_S  | 0 << WU_CLOCKS_BU_OSC32K_PD_S;


}


void exit(int exit_code)
{
    TEST_DONE();
    xputc(0x04);        // 0x04 - EOT - End Of Transmission 
    WaitTransmissionEnd();
    while (1) ;
}



void __attribute__((weak)) trap_handler()
{
    xprintf("\n\ntrap_handler\n");
    xprintf("mstatus: 0x%08x\n", read_csr(mstatus));
    xprintf("mtvec: 0x%08x\n", read_csr(mtvec));
    xprintf("mepc: 0x%08x\n", read_csr(mepc));
    xprintf("mcause: 0x%08x\n", read_csr(mcause));
    xprintf("mtval: 0x%08x\n", read_csr(mtval));
    
    exit(0);
}

void CheckTrapHandlerCalledOnce(int trap_handler_called)
{
    if (trap_handler_called != 1)
    {
        TEST_ERROR("trap_handler called %d times. Expected one time.", 
            trap_handler_called);
        DisableInterrupts();
    }
}

void EnableInterrupts()
{   
    set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}

void DisableInterrupts()
{    
    clear_csr(mie, MIE_MEIE);
}
    

void EnableAllClocks()
{
    PM->CLK_APB_P_SET = 0xFFFFFFFF;
    PM->CLK_APB_M_SET = 0xFFFFFFFF;
    PM->CLK_AHB_SET   = 0xFFFFFFFF;
}


void DisableAllClocks()
{
    PM->CLK_APB_M_CLEAR = 0xFFFFFFFF;
    PM->CLK_APB_P_CLEAR = 0xFFFFFFFF;
    PM->CLK_AHB_CLEAR   = 0xFFFFFFFF;
}

void Port0_As_Gpio ()
{
    PAD_CONFIG->PORT_0_CFG = 0x55555555;
}
void Port1_As_Gpio ()
{
    PAD_CONFIG->PORT_1_CFG = 0x55555555;
}
void Port2_As_Gpio ()
{
    PAD_CONFIG->PORT_2_CFG = 0x55555555;
}
void Port0_As_Func1 ()
{
    PAD_CONFIG->PORT_0_CFG = 0x00000000;
}
void Port1_As_Func1 ()
{
    PAD_CONFIG->PORT_1_CFG = 0x00000000;
}
void Port2_As_Func1 ()
{
    PAD_CONFIG->PORT_2_CFG = 0x00000000;
}

void Port0_As_Func2 ()
{
    PAD_CONFIG->PORT_0_CFG = 0xAAAAAAAA;
}
void Port1_As_Func2 ()
{
    PAD_CONFIG->PORT_1_CFG = 0xAAAAAAAA;
}
void Port2_As_Func2 ()
{
    PAD_CONFIG->PORT_2_CFG = 0xAAAAAAAA;
}

void Port0_As_Func3 ()
{
    PAD_CONFIG->PORT_0_CFG = 0xFFFFFFFF;
}
void Port1_As_Func3 ()
{
    PAD_CONFIG->PORT_1_CFG = 0xFFFFFFFF;
}
void Port2_As_Func3 ()
{
    PAD_CONFIG->PORT_2_CFG = 0xFFFFFFFF;
}

void Port0_As_Gpio ();
void Port1_As_Gpio ();
void Port2_As_Gpio ();
void Port0_As_Func1 ();
void Port1_As_Func1 ();
void Port2_As_Func1 ();
void Port0_As_Func2 ();
void Port1_As_Func2 ();
void Port2_As_Func3 ();
void Port0_As_Func3 ();
void Port1_As_Func3 ();
void Port2_As_Func2 ();
