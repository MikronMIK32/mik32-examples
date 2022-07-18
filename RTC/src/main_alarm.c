
// #include "common.h"
// #include "rtc.h"
// #include "power_manager.h"
// #include "wakeup.h"
// #include "epic.h"


// volatile uint32_t trap_handler_called = 0;
// uint32_t epic_index;
// uint32_t epic_mask;


// void trap_handler()
// {
//     //xprintf("trap_handler()\n");  // may mess up with main output
    
//     trap_handler_called++;
//     CheckTrapHandlerCalledOnce(trap_handler_called);
    
//     if ((EPIC->RAW_STATUS & epic_mask) == 0)
//     {
//         TEST_ERROR("EIC source [%d] is zero unexpectedly", epic_index);
//     } 
    
//     RTC->CTRL = RTC_CTRL_FLAG_M;
    
//     if ((EPIC->RAW_STATUS & epic_mask) != 0)
//     {
//         TEST_ERROR("EPIC source [%d] has not been cleared", epic_index);
//     } 
    
//     EPIC->CLEAR = epic_mask;    // clear flag in EPIC
// }


// void WaitFlag()
// {
//     uint32_t retry_limit = 10000;
//     for (uint32_t i = 0; i < retry_limit; i++)
//     {
//         if ((RTC->CTRL & RTC_CTRL_FLAG_M) == 0)
//         {
//             return;
//         }
//     }
    
//     TEST_ERROR("CTRL.FLAG is still 1 after %d retries", retry_limit);
// }


// int main()
// {        
//     uint32_t value;
//     int expected_counter = 0;
    
//     EPIC->MASK_SET = 0xFFFFFFFF;
//     EnableInterrupts();
    
//     epic_index = EPIC_RTC_INDEX;
//     epic_mask = 1 << epic_index;
    
//     *EdgeCounter_p = 400;
    
//     WU->CLOCKS_BU= 0<<WU_CLOCKS_BU_OSC32K_PD_S | 0<<WU_CLOCKS_BU_LSI32K_PD_S| WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
//     for (volatile int i = 0; i < 100; i++) {}; //two clocks need for glitch free  mux switch
//     WU->CLOCKS_BU= 0<<WU_CLOCKS_BU_OSC32K_PD_S | WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
//     xprintf("Run with OSC32K\n");
    
//     value = (7 << RTC_TIME_DOW_S) + 
//             (2 << RTC_TIME_TH_S) + 
//             (3 << RTC_TIME_H_S) + 
//             (5 << RTC_TIME_TM_S) + 
//             (9 << RTC_TIME_M_S) + 
//             (5 << RTC_TIME_TS_S) + 
//             (9 << RTC_TIME_S_S) + 
//             (8 << RTC_TIME_TOS_S);

//     xprintf("Load time and alarm time to RTC\n");
//     RTC->TIME = value;
//     WaitFlag();

//     // Alarm on 00s
//     RTC->TALRM = RTC_TALRM_CS_M | (value ^ (9 << RTC_TIME_S_S) ^ (5 << RTC_TIME_TS_S));

//     xprintf("Enable RTC\n");
//     RTC->CTRL = RTC_CTRL_EN_M | RTC_CTRL_INTE_M;
//     WaitFlag();

//     xprintf("Wait some time and check rtc_alarm_pad\n");
//     // Necessary delay depends on RTC clock (clocking pad model, 
//     // testbench input clock, some acceleration defines, ...)
//     for (volatile int i = 0; i < 10000; i++) ;    

//     expected_counter = 2;
//     if (*EdgeCounter_p != expected_counter)
//     {
//         TEST_ERROR("rtc_alarm_pad switched %d time(s) instead of %d",
//             *EdgeCounter_p, expected_counter);
//     }
    
//     CheckTrapHandlerCalledOnce(trap_handler_called);
// }


