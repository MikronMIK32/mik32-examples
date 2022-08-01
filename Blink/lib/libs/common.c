#define UART_OUTPUT
#include <common.h>
#include "csr.h"
#include "scr1_csr_encoding.h"
#include "uart_lib.h"

#ifndef UART_0_DIVIDER
    #define UART_0_DIVIDER 3333 //16
#endif


volatile uint32_t* EdgeCounter_p;
volatile uint32_t* Dac0Monitor_p;
volatile uint32_t* Dac1Monitor_p;
volatile uint32_t* PeriodMeasurer_p;
volatile uint32_t* RealValueGenerator_p;
volatile uint32_t* PowerGenerator_p;

void xputc (char c) 
{
#ifdef UART_OUTPUT
    UART_WriteByte(UART_0, c);
    UART_WaitTransmission(UART_0);
#else
    *((volatile uint32_t*)0x0008FFE4) = c;
#endif
}


void SystemInit()
{
    GPIO_2->OUTPUT = 0x2F; //GPIO CLK enable by JTAG 

    EnableAllClocks();
    for (volatile int i = 0; i < 10; i++) ;    
        
    EdgeCounter_p           = (uint32_t*)0x0008FFFC;
    Dac0Monitor_p           = (uint32_t*)0x0008FFF0;
    Dac1Monitor_p           = (uint32_t*)0x0008FFF4;
    PeriodMeasurer_p        = (uint32_t*)0x0008FFF8;
    RealValueGenerator_p    = (uint32_t*)0x0008FFEC;
    PowerGenerator_p        = (uint32_t*)0x0008FFE8;

    UART_Init(UART_0, UART_0_DIVIDER, 
        UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    


}


void exit()
{
    TEST_DONE();
    xputc(0x04);        // 0x04 - EOT - End Of Transmission 
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
    
    exit();
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
void Port2_As_Func2 ();
