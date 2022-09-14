#include "mcu32_memory_map.h"
#include "wakeup.h"
#include "power_manager.h"

/* Источники тактирования */
#define RCC_OSCILLATORTYPE_NONE            0b0000
#define RCC_OSCILLATORTYPE_HSI32M          0b0001
#define RCC_OSCILLATORTYPE_OSC32M          0b0010
#define RCC_OSCILLATORTYPE_LSI32K          0b0100
#define RCC_OSCILLATORTYPE_OSC32K          0b1000

/* Источники тактирования RTC */
#define RCC_RTCCLKSOURCE_NO_CLK            0b00          /* Нет источника */
#define RCC_RTCCLKSOURCE_LSI32K            0b01          /* LSI32K - источник тактироования RTC */
#define RCC_RTCCLKSOURCE_OSC32K            0b10          /* OSC32K - источник тактироования RTC */

/* Источник тактирования RTC в составе ядра*/
#define RCC_RTCCLKCPUSOURCE_NO_CLK         0b00          /* Нет источника */
#define RCC_RTCCLKCPUSOURCE_LSI32K         0b01          /* LSI32K - источник тактироования RTC */
#define RCC_RTCCLKCPUSOURCE_OSC32K         0b10          /* OSC32K - источник тактироования RTC */

#define PMCLOCKAHB_DEFAULT                 0x1F          /* Ядро; RAM; EEPROm; SPIFI; DMA */
#define PMCLOCKAPB_M_DEFAULT               0x89          /* Блок управления питанием (PM - Power Monitor); Контроллер выводов; RTC батарейного домена*/
#define PMCLOCKAPB_P_DEFAULT               0x00

#define PM_CPU_RTC_CLK_MUX_OSC32K          0x00          /* OSC32K -источник тактиования RTC в составе ядра*/
#define PM_CPU_RTC_CLK_MUX_LSI32K          0x01          /* LSI32K -источник тактиования RTC в составе ядра*/

typedef struct
{
    uint8_t OscillatorEnable;           /*  Осцилятор, который должен быть включен.
                                            Этот параметр может быть RCC_OSCILLATORTYPE_NONE,  RCC_OSCILLATORTYPE_HSI32M,
                                            RCC_OSCILLATORTYPE_LSI32K, RCC_OSCILLATORTYPE_OSC32K  */

    uint8_t OscillatorSystem;              /*  Источник тактирования системы.
                                            Этот парамерт должен быть числом между Min = 0 и Max = 255 */        

    uint8_t AHBDivider;                 /*  Делитель частоты AHB.
                                            Этот парамерт должен быть числом между Min = 0 и Max = 255 */

    uint8_t APBMDivider;                /*  Делитель частоты APB_M.
                                            Этот парамерт должен быть числом между Min = 0 и Max = 255 */

    uint8_t APBPDivider;                /*  Делитель частоты APB_P.
                                            Этот парамерт должен быть числом между Min = 0 и Max = 255 */
    
    uint8_t HSI32MCalibrationValue;     /*  Поправочный коэффициент HSI32M.
                                            Этот парамерт должен быть числом между Min = 0 и Max = 63 */

    uint8_t LSI32KCalibrationValue;     /*  Поправочный коэффициент LSI32K.
                                            Этот парамерт должен быть числом между Min = 0 и Max = 15 */

} RCC_OscInitTypeDef;


typedef struct
{
    uint32_t PMClockAHB;                /*  Выбор тактирования устройств на шине AHB. */

    uint32_t PMClockAPB_M;              /*  Выбор тактирования устройств на шине APB_M. */

    uint32_t PMClockAPB_P;              /*  Выбор тактирования устройств на шине APB_P. */

    uint32_t RTCClockSelection;         /*  Источник тактирования RTC. */

    uint32_t RTCClockCPUSelection;      /*  Источник тактирования RTC в составе ядра. */

} RCC_PeriphCLKInitTypeDef;

void HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInit);
void HAL_RCC_ClockConfig(RCC_PeriphCLKInitTypeDef *PeriphClkInit);