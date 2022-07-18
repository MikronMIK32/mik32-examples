
// #include "common.h"
// #include "rtc.h"
// #include "wakeup.h"
// #include "power_manager.h"
// #include "epic.h"




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
    
//     uint32_t epic_index;
//     uint32_t epic_mask;
//     uint32_t read_value, expected_value;
    
//     PM->CLK_APB_M_SET = PM_CLOCK_EPIC_M;
//     PM->CLK_APB_M_SET = PM_CLOCK_RTC_M;
//     PM->CLK_APB_P_SET = PM_CLOCK_WDT_M;
//     PM->CLK_APB_M_SET = PM_CLOCK_WU_M;

//     for (volatile int test_run =0;test_run<2;test_run++) 
//     {
//         if (test_run == 0)   
//         {       
//                 /*
//                 * CLOCKS_BU - Регистр управления тактированием батарейного домена
//                 * 
//                 * OCS32K_en - Включение/отключение внешнего осцилятора на 32 KГц - 0
//                 * RC32K_en - Включение/отключение LSI32К - 1
//                 * Adj_RC32К - Поправочные коэффициенты LSI32К - 6-9
//                 * RTC_Clk_Mux - Выбор источника тактирования часов реального времени: - 10
//                 *               0 – внутренний LSI32К;
//                 *               1 – внешний осциллятор OSC32K
//                 * OSC32K_sm - Режим повышенного потребления, активный уровень “0” для OSC32K - 14
//                 */
                
//                 WU->CLOCKS_BU= 0<<WU_CLOCKS_BU_OSC32K_PD_S | 0<<WU_CLOCKS_BU_LSI32K_PD_S| WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
//                 for (volatile int i = 0; i < 100; i++) {}; //two clocks need for glitch free  mux switch
//                 WU->CLOCKS_BU= 0<<WU_CLOCKS_BU_OSC32K_PD_S | WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
//                 xprintf("Run with OSC32K\n");
//         }
//         if (test_run == 1)   
//         {
//                 WU->CLOCKS_BU= 0<<WU_CLOCKS_BU_OSC32K_PD_S | 0<<WU_CLOCKS_BU_LSI32K_PD_S |  WU_CLOCKS_BU_RTC_CLK_MUX_LSI32K_M;
//                 for (volatile int i = 0; i < 100; i++) {}; //two clocks need for glitch free  mux switch
//                 WU->CLOCKS_BU= 0<<WU_CLOCKS_BU_LSI32K_PD_S |  WU_CLOCKS_BU_RTC_CLK_MUX_LSI32K_M;
//                 xprintf("Run with LSI32K_S\n");
//         }

//         WU->RTC_CONRTOL = WU_RTC_RESET_CLEAR_M;

    
//         xprintf("Disable RTC\n");
//         RTC->CTRL = 0;
//         WaitFlag();

//         expected_value =    (7 << RTC_TIME_DOW_S) + // вс
//                             (2 << RTC_TIME_TH_S) + // 
//                             (3 << RTC_TIME_H_S) + // 23ч 
//                             (5 << RTC_TIME_TM_S) + //
//                             (9 << RTC_TIME_M_S) + // 59м 
//                             (5 << RTC_TIME_TS_S) + //
//                             (9 << RTC_TIME_S_S) + // 59с
//                             (8 << RTC_TIME_TOS_S); // 

//         xprintf("  Load time to RTC\n");
//         RTC->TIME = expected_value;
//         WaitFlag();

//         read_value = RTC->TIME;
//         if (read_value != expected_value)
//         {
//             TEST_ERROR("Unexpected TIME register value read: 0x%08x Expected: 0x%08x", read_value, expected_value);
//         }

//         xprintf("  Load time alarm to RTC\n");
//         // Alarm on 00s
//         RTC->TALRM = RTC_TALRM_CS_M | (expected_value ^ (9 << RTC_TIME_S_S) ^ (5 << RTC_TIME_TS_S));

//         expected_value =    (1 << RTC_DATE_TC_S) + 
//                             (9 << RTC_DATE_C_S) + 
//                             (9 << RTC_DATE_TY_S) + 
//                             (9 << RTC_DATE_Y_S) + 
//                             (1 << RTC_DATE_TM_S) + 
//                             (2 << RTC_DATE_M_S) +
//                             (3 << RTC_DATE_TD_S) + 
//                             (1 << RTC_DATE_D_S);

//         xprintf("  Load date to RTC\n");
//         RTC->DATE = expected_value;
//         WaitFlag();

//         read_value = RTC->DATE;
//         if (read_value != expected_value)
//         {
//             TEST_ERROR("Unexpected DATE register value read: 0x%08x Expected: 0x%08x", read_value, expected_value);
//         }

//         xprintf("  Enable RTC\n");
//         RTC->CTRL = RTC_CTRL_EN_M | RTC_CTRL_INTE_M;
//         WaitFlag();


//         xprintf("  Wait some time\n");
//         // Necessary delay depends on RTC clock (clocking pad model, 
//         // testbench input clock, some acceleration defines, ...)
//         for (volatile int i = 0; i < 10000; i++) ;    
//         read_value = RTC->TIME;
        
//         xprintf("  Check EPIC line\n");
//         epic_index = EPIC_RTC_INDEX;
//         epic_mask = (1 << epic_index);
//         if ((EPIC->RAW_STATUS & epic_mask) == 0)
//         {
//             TEST_ERROR("EPIC source [%d] is zero unexpectedly", epic_index);
//         } 
//         xprintf("  Reset EPIC line\n");
//         RTC->CTRL = RTC_CTRL_EN_M;
//         if ((EPIC->RAW_STATUS & epic_mask) != 0)
//         {
//             TEST_ERROR("EPIC source [%d] is one unexpectedly", epic_index);
//         } 

//         xprintf("  Check RTC values\n");
//         expected_value =    (1 << RTC_TIME_DOW_S);
//         read_value &= ~(0xF << RTC_TIME_TOS_S);
//         if (read_value != expected_value)
//         {
//             TEST_ERROR("Unexpected TIME register value read: 0x%08x Expected: 0x%08x", read_value, expected_value);
//         }

//         expected_value =    (2 << RTC_DATE_TC_S) + 
//                             (0 << RTC_DATE_C_S) + 
//                             (0 << RTC_DATE_TY_S) + 
//                             (0 << RTC_DATE_Y_S) + 
//                             (0 << RTC_DATE_TM_S) + 
//                             (1 << RTC_DATE_M_S) +
//                             (0 << RTC_DATE_TD_S) + 
//                             (1 << RTC_DATE_D_S);
//         read_value = RTC->DATE;
//         if (read_value != expected_value)
//         {
//             TEST_ERROR("Unexpected DATE register value read: 0x%08x Expected: 0x%08x", read_value, expected_value);
//         }

//         for (volatile int i = 0; i < 400; i++) ;    
//         xprintf("  Reset RTC\n");

        
//         WU->RTC_CONRTOL = WU_RTC_RESET_SET_M;
//         WU->RTC_CONRTOL = WU_RTC_RESET_CLEAR_M;
        
//         // Reset needs to be synchronised
//         for (volatile int i = 0; i < 500; i++) ; 

//         read_value = RTC->CTRL;
//         if (read_value != 0)
//         {
//             TEST_ERROR("Unexpected CTRL register value read: 0x%08x Expected: 0x%08x", read_value, 0);
//         }
        
//         read_value = RTC->DATE;
//         if (read_value != 0)
//         {
//             TEST_ERROR("Unexpected DATE  register value read: 0x%08x Expected: 0x%08x", read_value, 0);
//         }
        
//         read_value = RTC->TIME;
//         if (read_value != 0x01000000)
//         {
//             TEST_ERROR("Unexpected TIME register value read: 0x%08x Expected: 0x%08x", read_value, 0x01000000);
//         }
//   } 
//    exit ();
// }


