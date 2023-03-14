#ifndef MIK32_HAL_I2C
#define MIK32_HAL_I2C

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "spi.h"
#include "stdbool.h"

#ifdef MIK32_SPI_DEBUG
#include "common.h"
#endif


/* Title: Макросы */

/*
 * Defines: Выбор ведомых устройств 
 *
 * SPI_CS_NONE - Ведомое устройство не выбрано;
 * SPI_CS_0    - Ведомое устройство 1;
 * SPI_CS_1    - Ведомое устройство 2;
 * SPI_CS_2    - Ведомое устройство 3;
 * SPI_CS_3    - Ведомое устройство 4.
 *
 */
/* SPI_CS - Выбор ведомых устройств */
#define SPI_CS_NONE   0xF      /* Ведомое устройство не выбрано */
#define SPI_CS_0      0xE      /* Ведомое устройство 1 */
#define SPI_CS_1      0xD      /* Ведомое устройство 2 */
#define SPI_CS_2      0xB      /* Ведомое устройство 3 */
#define SPI_CS_3      0x7      /* Ведомое устройство 4 */

/*
 * Defines: Делитель частоты
 *
 * Коэффициент деления частоты spi_ref_clk
 *
 * SPI_BAUDRATE_DIV4   - Деление на 4;
 * SPI_BAUDRATE_DIV8   - Деление на 8;
 * SPI_BAUDRATE_DIV16  - Деление на 16;
 * SPI_BAUDRATE_DIV32  - Деление на 32;
 * SPI_BAUDRATE_DIV64  - Деление на 64;
 * SPI_BAUDRATE_DIV128 - Деление на 128;
 * SPI_BAUDRATE_DIV256 - Деление на 256.
 *
 */
/* SPI_BaudRateDiv - Коэффициент деления частоты spi_ref_clk */
#define SPI_BAUDRATE_DIV4       0b001   /* Деление на 4 */
#define SPI_BAUDRATE_DIV8       0b010   /* Деление на 8 */
#define SPI_BAUDRATE_DIV16      0b011   /* Деление на 16 */
#define SPI_BAUDRATE_DIV32      0b100   /* Деление на 32 */
#define SPI_BAUDRATE_DIV64      0b101   /* Деление на 64 */
#define SPI_BAUDRATE_DIV128     0b110   /* Деление на 128 */
#define SPI_BAUDRATE_DIV256     0b111   /* Деление на 256 */

/*
 * Defines: Ручной режим
 *
 * Режим управления сигналом выбора ведомого CS
 *
 * SPI_MANUALCS_OFF - Автоматический режим;
 * SPI_MANUALCS_ON  - Ручной режим.
 *
 */
/* SPI_ManualCS - Режим управления сигналом выбора ведомого CS */
#define SPI_MANUALCS_OFF      0   /* Автоматический режим */
#define SPI_MANUALCS_ON       1   /* Ручной режим */

/*
 * Defines: Фаза тактового сигнала
 *
 * Настройки фазы тактового сигнала
 *
 * SPI_PHASE_OFF - Тактовая частота SPI активна вне слова;
 * SPI_PHASE_ON  - Тактовая частота SPI неактивна вне слова.
 *
 */
/* SPI_CLKPhase - Настройки фазы тактового сигнала */
#define SPI_PHASE_OFF   0  /* Тактовая частота SPI активна   вне слова */
#define SPI_PHASE_ON    1  /* Тактовая частота SPI неактивна вне слова */

/*
 * Defines: Полярность тактового сигнала
 *
 * Настройки полярности тактового сигнала вне слова
 *
 * SPI_POLARITY_LOW  - Тактовый сигнал вне слова удерживается на низком  уровне;
 * SPI_POLARITY_HIGH - Тактовый сигнал вне слова удерживается на высоком уровне.
 *
 */
/* SPI_CLKPolarity - Настройки полярности тактового сигнала вне слова */
#define SPI_POLARITY_LOW    0   /* Тактовый сигнал вне слова удерживается на низком  уровне */
#define SPI_POLARITY_HIGH   1   /* Тактовый сигнал вне слова удерживается на высоком уровне */

/*
 * Defines: Внешний декодер
 *
 * SPI_DECODER_NONE - Внешний декодер не используется. Выбор только 1 из 4-х ведомых устройств;
 * SPI_DECODER_USE  - Используется внешний декодер.
 *
 */
/* SPI_Decoder - Использование внешнего декодера */
#define SPI_DECODER_NONE     0   /* Внешний декодер не используется. Выбор только 1 из 4 ведомых устройств */
#define SPI_DECODER_USE      1   /* Используется внешний декодер */

/*
 * Defines: Длина передаваемой посылки
 *
 * Определяет количество байт, при пересылке которых CS не будет менять свое состояние.
 *
 * SPI_DATASIZE_8BITS  - Длина передаваемой посылки 8 бит;
 * SPI_DATASIZE_16BITS - Длина передаваемой посылки 16 бит;
 * SPI_DATASIZE_24BITS - Длина передаваемой посылки 24 бит;
 * SPI_DATASIZE_32BITS - Длина передаваемой посылки 32 бит.
 *
 */
/* SPI_DataSizey - Длина передаваемой посылки */
#define SPI_DATASIZE_8BITS      0   /* Длина передаваемой посылки - 8 бит */
#define SPI_DATASIZE_16BITS     1   /* Длина передаваемой посылки - 16 бит */
#define SPI_DATASIZE_24BITS     2   /* Длина передаваемой посылки - 24 бит */
#define SPI_DATASIZE_32BITS     3   /* Длина передаваемой посылки - 32 бит */

/* SPI_Error - Ошибки SPI */
#define SPI_ERROR_RXOVR_OFF         (uint8_t)0
#define SPI_ERROR_RXOVR_ON          (uint8_t)1
#define SPI_ERROR_ModeFail_OFF      (uint8_t)0
#define SPI_ERROR_ModeFail_ON       (uint8_t)1

/* Значения по умолчанию */
#define SPI_THRESHOLD_DEFAULT   1   /* Значение Threshold_of_TX_FIFO по умолчанию*/


/* Title: Перечисления */

/*
 * Enum: HAL_SPI_ModeTypeDef
 * Выбор режима ведущий/ведомый
 *
 * HAL_SPI_MODE_SLAVE - Режим ведомого.
 * HAL_SPI_MODE_MASTER - Режим ведущего.
 *
 */
typedef enum 
{
    HAL_SPI_MODE_SLAVE = 0,     /* Режим ведомого */
    HAL_SPI_MODE_MASTER = 1,    /* Режим ведущего */

} HAL_SPI_ModeTypeDef;


/* Title: Структуры */

/*
 * Struct: HAL_SPI_ErrorTypeDef
 * Ошибки при работе SPI
 * 
 */
typedef struct 
{
    /*
    * Variable: RXOVR
    * Переполнение RX_FIFO
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_ERROR_RXOVR_OFF>;
    * - <SPI_ERROR_RXOVR_ON>. 
    *
    */
    uint8_t RXOVR;

    /*
    * Variable: ModeFail
    * Напряжение на выводе n_ss_in не соответствую режиму работы SPI
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_ERROR_ModeFail_OFF>;
    * - <SPI_ERROR_ModeFail_ON>. 
    *
    */
    uint8_t ModeFail;

} HAL_SPI_ErrorTypeDef;

/*
 * Struct: SPI_InitTypeDef
 * Настройки инициализации SPI
 * 
 */
typedef struct
{
    /*
    * Variable: SPI_Mode
    * Режим SPI (ведомый/ведущий)
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <HAL_SPI_MODE_SLAVE>;
    * - <HAL_SPI_MODE_MASTER>. 
    *
    */
    HAL_SPI_ModeTypeDef SPI_Mode;   /* Режим SPI */
    
    /*
    * Variable: BaudRateDiv
    * Коэффициент деления частоты
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_BAUDRATE_DIV4>
    * - <SPI_BAUDRATE_DIV8>
    * - <SPI_BAUDRATE_DIV16>
    * - <SPI_BAUDRATE_DIV32>
    * - <SPI_BAUDRATE_DIV64>
    * - <SPI_BAUDRATE_DIV128>
    * - <SPI_BAUDRATE_DIV256>
    *
    */
    uint8_t BaudRateDiv;            /* Коэффициент деления частоты */ 

    /*
    * Variable: ManualCS
    * Режим управления сигналом выбора ведомого CS
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_MANUALCS_OFF>;
    * - <SPI_MANUALCS_ON>.
    *
    */
    uint8_t ManualCS;               /* Режим управления сигналом выбора ведомого - CS */

    /*
    * Variable: CLKPhase
    * Настройка фазы тактового сигнала
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_PHASE_OFF>;
    * - <SPI_PHASE_ON>.
    *
    */
    uint8_t CLKPhase;               /* Настройка фазы тактового сигнала */

    /*
    * Variable: CLKPolarity
    * Настройка полярности тактового сигнала вне слова
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_POLARITY_LOW>;
    * - <SPI_POLARITY_HIGH>.
    *
    */
    uint8_t CLKPolarity;            /* Настройка полярности тактового сигнала вне слова */

    /*
    * Variable: Decoder
    * Использование внешнего декодера
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_DECODER_NONE>;
    * - <SPI_DECODER_USE>.
    *
    */
    uint8_t Decoder;                /* Использование внешнего декодера */

    /*
    * Variable: DataSize
    * Длина передаваемой посылки
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_DATASIZE_8BITS>;
    * - <SPI_DATASIZE_16BITS>;
    * - <SPI_DATASIZE_24BITS>;
    * - <SPI_DATASIZE_32BITS>;
    *
    */
    uint8_t DataSize;               /* Длина передаваемой посылки */

} SPI_InitTypeDef;

/*
 * Struct: SPI_HandleTypeDef
 * Настройки инициализации SPI
 * 
 */
typedef struct
{
   /*
    * Variable: Instance
    * Базоый адрес регистров SPI
    *
    */
    SPI_TypeDef *Instance;

    /*
    * Variable: Init
    * Параметры SPI
    *
    */
    SPI_InitTypeDef Init;

    /*
    * Variable: Error
    * Ошибки при работе SPI
    *
    */
    HAL_SPI_ErrorTypeDef Error;

    /*
    * Variable: ChipSelect
    * Выбранное ведомое устройство
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <SPI_CS_NONE>;
    * - <SPI_CS_0>;
    * - <SPI_CS_1>;
    * - <SPI_CS_2>;
    * - <SPI_CS_3>.
    *
    */
    uint8_t ChipSelect;
    
} SPI_HandleTypeDef;


/* Title: Функции */

/*
 * Function: HAL_SPI_Enable
 * Включить модуль SPI
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_Enable(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_Disable
 * Выключить модуль SPI
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_Disable(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_ClearRxBuffer
 * Читать из RX_FIFO до опустошения
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_ClearRxBuffer(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_IntEnable
 * Разрешить прерывания в соответсвии с маской int_en 
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * int_en - Маска прерываний
 *
 * Returns:
 * void
 */
void HAL_SPI_IntEnable(SPI_HandleTypeDef *hspi, uint32_t int_en);

/*
 * Function: HAL_SPI_IntDisable
 * Запретить прерывания в соответсвии с маской int_dis
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * int_dis - Маска прерываний
 *
 * Returns:
 * void
 */
void HAL_SPI_IntDisable(SPI_HandleTypeDef *hspi, uint32_t int_dis);

/*
 * Function: HAL_SPI_SetDelayBTWN
 * Задать задержку BTWN
 * 
 * Задержка в периодах опорного тактового сигнала или внешнего тактового сигнала ext_clk 
 * между снятием сигнала выбора одного ведомого устройства и установкой сигнала выбора другого ведомого устройства.
 * 
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * btwn - Задержка BTWN
 *
 * Returns:
 * void
 */
void HAL_SPI_SetDelayBTWN(SPI_HandleTypeDef *hspi, uint8_t btwn);

/*
 * Function: HAL_SPI_SetDelayAFTER
 * Задать задержку AFTER
 * 
 * Задержка в периодах опорного тактового сигнала или внешнего тактового сигнала ext_clk между последним битом текущего
 * слова и первым битом следующего слова.
 * 
 * Этот параметр может быть числом в пределах от 0 до 255.
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * after - Задержка AFTER
 *
 * Returns:
 * void
 */
void HAL_SPI_SetDelayAFTER(SPI_HandleTypeDef *hspi, uint8_t after);

/*
 * Function: HAL_SPI_SetDelayINIT
 * Задать задержку INIT
 * 
 * Дополнительная задержка в периодах опорного тактового сигнала или внешнего тактового сигнала ext_clk между установкой 
 * сигнала n_ss_out в «0» и передачей первого бита.
 * 
 * Этот параметр может быть числом в пределах от 0 до 255.
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * init - Задержка INIT
 *
 * Returns:
 * void
 */
void HAL_SPI_SetDelayINIT(SPI_HandleTypeDef *hspi, uint8_t init);

/*
 * Function: HAL_SPI_SetSlaveIdleCounter
 * Задать задержку перед передачей
 * 
 * Модуль SPI в режиме ведомого устройства начинает передачу только когда тактовый сигнал sclk_in (внешнего ведущего устройства)
 * не изменяется в течение количества периодов опорного тактового сигнала SPI заданного в этом поле или когда модуль SPI не активен.
 * 
 * Этот параметр может быть числом в пределах от 0 до 255.
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * slave_idle_counter - Задержка в тактах сигнала sclk_in
 *
 * Returns:
 * void
 */
void HAL_SPI_SetSlaveIdleCounter(SPI_HandleTypeDef *hspi, uint8_t slave_idle_counter);

/*
 * Function: HAL_SPI_SetThresholdTX
 * Задать ThresholdTX
 * 
 * Задает уровень, при котором TX_FIFO считается не заполненным и формируется прерывание.
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 * threshold - Уровень, при котором TX_FIFO считается не заполненным.
 *
 * Returns:
 * void
 */
void HAL_SPI_SetThresholdTX(SPI_HandleTypeDef *hspi, uint32_t threshold);

/*
 * Function: HAL_SPI_ReadModuleID
 * Получить идентификационный номер модуля
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * (uint32_t ) - Идентификационный номер модуля
 */
uint32_t HAL_SPI_ReadModuleID(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_Init
 * Инициализировать Crypto в соответствии с настройками <SPI_HandleTypeDef> *hspi
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_Init(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_ClearTXFIFO
 * Очистить TX_FIFO
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_ClearTXFIFO(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_ClearRXFIFO
 * Очистить RX_FIFO
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_ClearRXFIFO(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_ClearError
 * Сбросить состояния ошибок
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_ClearError(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_CheckError
 * Проверить наличие ошибок
 * 
 * Если ошибки были найдены, то их статусы сбрасываются, а RX_FIFO и TX_FIFO очищаются.
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_CheckError(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_WaitTxNotFull
 * Ожидать пока регистр TX_FIFO заполнен меньше чем THRESHOLD;
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_WaitTxNotFull(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_WaitRxNotEmpty
 * Ожидать пока в RX_FIFO есть хотя бы один байт
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi
 *
 * Returns:
 * void
 */
void HAL_SPI_WaitRxNotEmpty(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_CS_Enable
 * Перевести CS_M в активное состояние
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi;
 * CS_M - Выбор ведомых устройств.
 * Этот параметр должен быть одним из значений:
 * 
 * - <SPI_CS_NONE>;
 * - <SPI_CS_0>;
 * - <SPI_CS_1>;
 * - <SPI_CS_2>;
 * - <SPI_CS_3>.
 *
 * Returns:
 * void
 */
void HAL_SPI_CS_Enable(SPI_HandleTypeDef *hspi, uint32_t CS_M);

/*
 * Function: HAL_SPI_CS_Disable
 * Перевести активный CS в неактивное состояние
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi;
 * Этот параметр должен быть одним из значений:
 * 
 * - <SPI_CS_NONE>;
 * - <SPI_CS_0>;
 * - <SPI_CS_1>;
 * - <SPI_CS_2>;
 * - <SPI_CS_3>.
 * 
 * Returns:
 * void
 */
void HAL_SPI_CS_Disable(SPI_HandleTypeDef *hspi);

/*
 * Function: HAL_SPI_Exchange
 * Запустить обмен данными
 *
 * Parameters:
 * hspi - Указатель на структуру с настройками spi;
 * transmit_bytes - Массив данных для отправки;
 * receive_bytes - Массив данных для приема;
 * count - количество байт данных
 * 
 * Returns:
 * void
 */
void HAL_SPI_Exchange(SPI_HandleTypeDef *hspi, uint8_t transmit_bytes[], uint8_t receive_bytes[], uint32_t count);

#endif