#ifndef MIK32_HAL_I2C
#define MIK32_HAL_I2C

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "spi.h"
#include "stdbool.h"

#ifdef MIK32_SPI_DEBUG
#include "common.h"
#endif

#define I2C_CR1_DNF_S                   8
#define I2C_CR1_DNF_M                   (0xF << I2C_CR1_DNF_S)
#define I2C_CR1_DNF(v)                  (((v) << I2C_CR1_DNF_S) & I2C_CR1_DNF_M)

/* Регистры SPI */
#define SPI_CONFIG_DATA_SZ_S		6

#define SPI_DELAY_BTWN_S            16
#define SPI_DELAY_BTWN_M            (0xFF << SPI_DELAY_BTWN_S)
#define SPI_DELAY_BTWN(v)           (((v) << SPI_DELAY_BTWN_S) & SPI_DELAY_BTWN_M)

#define SPI_DELAY_AFTER_S           8
#define SPI_DELAY_AFTER_M           (0xFF << SPI_DELAY_AFTER_S)
#define SPI_DELAY_AFTER(v)          (((v) << SPI_DELAY_AFTER_S) & SPI_DELAY_AFTER_M)

#define SPI_DELAY_INIT_S            0
#define SPI_DELAY_INIT_M            (0xFF << SPI_DELAY_INIT_S)
#define SPI_DELAY_INIT(v)           (((v) << SPI_DELAY_INIT_S) & SPI_DELAY_INIT_M)

/* SPI_CS - Выбор ведомых устройств */
#define SPI_CS_NONE   0xF      /* Ведомое устройство не выбрано */
#define SPI_CS_0      0xE      /* Ведомое устройство 1 */
#define SPI_CS_1      0xD      /* Ведомое устройство 2 */
#define SPI_CS_2      0xB      /* Ведомое устройство 3 */
#define SPI_CS_3      0x7      /* Ведомое устройство 4 */

/* SPI_BaudRateDiv - Коэффициент деления частоты spi_ref_clk */
#define SPI_BAUDRATE_DIV4       0b001   /* Деление на 4 */
#define SPI_BAUDRATE_DIV8       0b010   /* Деление на 8 */
#define SPI_BAUDRATE_DIV16      0b011   /* Деление на 16 */
#define SPI_BAUDRATE_DIV32      0b100   /* Деление на 32 */
#define SPI_BAUDRATE_DIV64      0b101   /* Деление на 64 */
#define SPI_BAUDRATE_DIV128     0b110   /* Деление на 128 */
#define SPI_BAUDRATE_DIV256     0b111   /* Деление на 256 */

/* SPI_ManualCS - Режимм управления сигналом выбора ведомого - CS */
#define SPI_MANUALCS_OFF      0   /* Ручной режим */
#define SPI_MANUALCS_ON       1   /* Автоматический режим */

/* SPI_CLKPhase - Настройки фазы тактового сигнала */
#define SPI_PHASE_OFF   0  /* Тактовая частота SPI активна   вне слова */
#define SPI_PHASE_ON    1  /* Тактовая частота SPI неактивна вне слова */

/* SPI_CLKPolarity - Настройки полярности тактового сигнала вне слова */
#define SPI_POLARITY_LOW    0   /* Тактовый сигнал вне слова удерживается на низком  уровне */
#define SPI_POLARITY_HIGH   1   /* Тактовый сигнал вне слова удерживается на высоком уровне */

/* SPI_CLKPolarity - Использование внешнего декодера */
#define SPI_DECODER_NONE     0   /* Внешний декодер не используется. Выбор только 1 из 4 ведомых устройств */
#define SPI_DECODER_USE      1   /* Используется внешний декодер */

/* SPI_DataSizey - Длина передаваемой посылки */
#define SPI_DATASIZE_8BITS      0   /* Длина передаваемой посылки - 8 бит */
#define SPI_DATASIZE_16BITS     1   /* Длина передаваемой посылки - 16 бит */
#define SPI_DATASIZE_24BITS     2   /* Длина передаваемой посылки - 24 бит */
#define SPI_DATASIZE_32BITS     3   /* Длина передаваемой посылки - 32 бит */

#define SPI_THRESHOLD_DEFAULT   1   /* Значение Threshold_of_TX_FIFO по умолчанию*/

typedef enum 
{
    HAL_SPI_MODE_SLAVE = 0,     /* Режим ведомого */
    HAL_SPI_MODE_MASTER = 1,    /* Режим ведущего */

} HAL_SPI_ModeTypeDef;

typedef struct
{
    HAL_SPI_ModeTypeDef SPI_Mode;   /* Режим SPI */
    
    uint8_t BaudRateDiv;            /* Коэффициент деления частоты */ 

    uint8_t ManualCS;               /* Режимм управления сигналом выбора ведомого - CS */

    uint8_t CLKPhase;               /* Настройки фазы тактового сигнала */

    uint8_t CLKPolarity;            /* Настройки полярности тактового сигнала вне слова */

    uint8_t Decoder;                /* Использование внешнего декодера */

    uint8_t DataSize;               /* Длина передаваемой посылки */

} SPI_InitTypeDef;

typedef struct
{
    SPI_TypeDef *Instance;          /* Базоый адрес регистров SPI */

    SPI_InitTypeDef Init;           /* Параметры SPI */

    uint8_t ChipSelect;             /* Выбранное ведомое устройство */
 
} SPI_HandleTypeDef;

void HAL_SPI_Enable(SPI_HandleTypeDef *hspi);
void HAL_SPI_Disable(SPI_HandleTypeDef *hspi);
void HAL_SPI_ClearRxBuffer(SPI_HandleTypeDef *hspi);
void HAL_SPI_IntEnable(SPI_HandleTypeDef *hspi, uint32_t int_en);
void HAL_SPI_IntDisable(SPI_HandleTypeDef *hspi, uint32_t int_dis);
void HAL_SPI_SetDelayBTWN(SPI_HandleTypeDef *hspi, uint8_t btwn);
void HAL_SPI_SetDelayAFTER(SPI_HandleTypeDef *hspi, uint8_t after);
void HAL_SPI_SetDelayINIT(SPI_HandleTypeDef *hspi, uint8_t init);
void HAL_SPI_SetSlaveIdleCounter(SPI_HandleTypeDef *hspi, uint8_t slave_idle_counter);
void HAL_SPI_SetThresholdTX(SPI_HandleTypeDef *hspi, uint32_t threshold);
uint32_t HAL_SPI_ReadModuleID(SPI_HandleTypeDef *hspi);
void HAL_SPI_Init(SPI_HandleTypeDef *hspi);
void HAL_SPI_WaitTxNotFull(SPI_HandleTypeDef *hspi);
void HAL_SPI_WaitRxNotEmpty(SPI_HandleTypeDef *hspi);
bool HAL_SPI_CheckOverflow(SPI_HandleTypeDef *hspi);
bool HAL_SPI_CheckModeFail(SPI_HandleTypeDef *hspi);
void HAL_SPI_CS_Enable(SPI_HandleTypeDef *hspi, uint32_t CS_M)  ;
void HAL_SPI_CS_Disable(SPI_HandleTypeDef *hspi)  ;
void HAL_SPI_Exchange_Fail(SPI_HandleTypeDef *hspi, uint8_t transmit_bytes[], uint8_t receive_bytes[], uint32_t count);
void HAL_SPI_Exchange(SPI_HandleTypeDef *hspi, uint8_t transmit_bytes[], uint8_t receive_bytes[], uint32_t count);

#endif