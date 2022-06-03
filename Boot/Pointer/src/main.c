#include "common.h"

void __attribute__((section(".boot"))) foo(void)
{
    xprintf("\nHello!\n");
}

extern uint32_t __boot_START__, __crt0_END__, __TEXT_START__, __TEXT_END__;

int main()
{
    uint32_t *pboot, *ptext_start, *ptext_end, *pcrt0_end;
    pboot = &__boot_START__;
    ptext_start = &__TEXT_START__;
    ptext_end = &__TEXT_END__;
    pcrt0_end = &__crt0_END__;

    xprintf("\n__TEXT_START__, адрес 0x%02x\n", ptext_start);
    xprintf("\n__crt0_END__, адрес 0x%02x\n", pcrt0_end);
    xprintf("\n__boot_START__, адрес 0x%02x\n", pboot);
    xprintf("\n__TEXT_END__, адрес 0x%02x\n", ptext_end);

    xprintf("\nАдрес foo() должен быть __boot_START__  =  0x%02x\n", pboot);

    xprintf("\nfoo(), адрес 0x%02x\n", foo);
    
    foo();
    while (1)
    {

    }
    
    
}