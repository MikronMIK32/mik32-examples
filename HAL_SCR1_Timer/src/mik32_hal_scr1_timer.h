#ifndef MIK32_HAL_SCR1_TIMER
#define MIK32_HAL_SCR1_TIMER

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "power_manager.h"

#include "csr.h"
#include "scr1_csr_encoding.h"
#include "scr1_timer.h"

#ifdef MIK32_SCR1_TIMER_DEBUG
#include "common.h"
#endif


#define MIK32_FREQ          32000000      /* Входная частота, Гц */
#define MIK32_FREQ_MHZ      32            /* Входная частота, МГц */

/* Источник тактированя */
#define SCR1_TIMER_CLKSRC_INTERNAL        0   /* Тактирование от ядра */
#define SCR1_TIMER_CLKSRC_EXTERNAL_RTC    1   /* Тактирование от внешнего RTC */



typedef struct
{
    SCR1_TIMER_TypeDef *Instance;       /* Базоый адрес регистров OTP */

    uint8_t ClockSource;    /* Источник тактирования */
    uint16_t Divider;       /* Делтитель частоты 10-битное число */
    
} OTP_HandleTypeDef;


void HAL_SCR1_Timer_Enable(OTP_HandleTypeDef *hscr1_timer);
void HAL_SCR1_Timer_Disable(OTP_HandleTypeDef *hscr1_timer);
void HAL_SCR1_Timer_SetClockSource(OTP_HandleTypeDef *hscr1_timer, uint8_t ClockSource);
void HAL_SCR1_Timer_SetDivider(OTP_HandleTypeDef *hscr1_timer, uint16_t Divider);
void HAL_SCR1_Timer_Init(OTP_HandleTypeDef *hscr1_timer);

/* Время задержки должено быть не больше 134217 мс */ 
void HAL_DelayMs(OTP_HandleTypeDef *hscr1_timer, uint32_t Milliseconds);

#endif

/*
* uint32_t TIMER_CTRL;
* uint32_t TIMER_DIV;
* uint32_t MTIME;
* uint32_t MTIMEH;    
* uint32_t MTIMECMP;
* uint32_t MTIMECMPH;
*/

/*

Регистр TIMER_CTRL
------------------------------------------------------------------------------------------------
Обозначение     |   Биты    |   По умолчанию    |   Описание                |   Доступ
------------------------------------------------------------------------------------------------
ENABLE          |   0       |   X               |   Включение таймера       |   RW
------------------------------------------------------------------------------------------------
CLKSRC          |   1       |   0x0             |   Источник тактирования   |   RW
                |           |                   |   0 - Тактирования ядра   |
                |           |                   |   1 - Внешний RTC         |  
------------------------------------------------------------------------------------------------
Reserved        |   31:2    |                                               |   RZ
------------------------------------------------------------------------------------------------


Регистр TIMER_DIV
------------------------------------------------------------------------------------------------
Обозначение     |   Биты    |   По умолчанию    |   Описание                |   Доступ
------------------------------------------------------------------------------------------------
DIV             |   9:0     |   X               |   Делитель                |   RW
                                                |   Счет идет каждые DIV+1  |
                                                |   такта частоты           |
------------------------------------------------------------------------------------------------
Reserved        |   31:10   |   X                                           |   RZ
------------------------------------------------------------------------------------------------


По умолчанию, MTIME и MTIMEH равны нулю после перезагрузки ядра (которая также
начинает подсчет). Другим вариантом начала счета для MTIME/MTIMEH является запись некоторого значения в
MTIME/MTIMEH.

MTIME - Счетчик таймера. 64-битне число. MTIMEH - старшее слово, MTIME - младшее слово.
MTIMECMP - Регистр сравнения. 64-битное число. MTIMECMPH - старшее слово, MTIMECMP - младшее слово.

Прерывание машинного таймера становится ожидающим всякий раз, когда mtime содержит значение, большее или равное mtimecmp, 
обрабатывая значения как целые числа без знака. Прерывание сохраняется до тех пор, пока mtimecmp не станет больше
mtime (обычно в результате записи mtimecmp). Прерывание будет принято только в том случае, если
включены прерывания и бит MTIE установлен в регистре mie.

*/
