#ifndef MIK32_HAL_I2C
#define MIK32_HAL_I2C

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "i2c.h"
#include "stdbool.h"

#ifdef MIK32_I2C_DEBUG
#include "common.h"
#endif

#define I2C_TIMEOUT 1000000
#define I2C_NBYTE_MAX 255
#define I2C_DATA_BYTES 1

/* I2C_error - номера ошибок I2C*/
#define I2C_ERROR_NONE 0
#define I2C_ERROR_TIMEOUT 1
#define I2C_ERROR_NACK 2
#define I2C_ERROR_BERR 3
#define I2C_ERROR_ARLO 4
#define I2C_ERROR_OVR 5

/* I2C_addressing_mode - Режим адреса */
#define I2C_ADDRESSINGMODE_7BIT 0
#define I2C_ADDRESSINGMODE_10BIT 1

/* I2C_dual_addressing_mode - Режим дополнительнрого адреса 7 бит */
#define I2C_DUALADDRESS_DISABLE 0
#define I2C_DUALADDRESS_ENABLE 1

/* I2C_nostretch_mode - Режим удержания SCL ведомого */
#define I2C_NOSTRETCH_DISABLE 0
#define I2C_NOSTRETCH_ENABLE 1

/* I2C_sbc_mode - Режим аппаратного контроля байта ведомым */
#define I2C_SBC_DISABLE 0
#define I2C_SBC_ENABLE 1

/* I2C_reload_mode - Режим аппаратного контроля байта ведомым */
#define I2C_RELOAD_DISABLE 0
#define I2C_RELOAD_ENABLE 1

/* I2C_address_shift - Сдвиг адреса ведомого на 1 бит влево */
#define SHIFT_ADDRESS_DISABLE 0
#define SHIFT_ADDRESS_ENABLE 1

/* I2C_autoend_mode - Режим автоматического окончания */
#define AUTOEND_DISABLE 0
#define AUTOEND_ENABLE 1

/* I2C_transfer_direction - Направление передачи */
#define I2C_TRANSFER_WRITE 0
#define I2C_TRANSFER_READ 1

/* I2C_OwnAddress2_mask - Маска второго собственного адреса */
#define I2C_OWNADDRESS2_MASK_DISABLE 0
#define I2C_OWNADDRESS2_MASK_111111x 1
#define I2C_OWNADDRESS2_MASK_11111xx 2
#define I2C_OWNADDRESS2_MASK_1111xxx 3
#define I2C_OWNADDRESS2_MASK_111xxxx 4
#define I2C_OWNADDRESS2_MASK_11xxxxx 5
#define I2C_OWNADDRESS2_MASK_1xxxxxx 6
#define I2C_OWNADDRESS2_MASK_1111111 7

/* I2C_filter - Цифровой фильтр*/
#define I2C_FILTER_OFF           0
#define I2C_FILTER_1CLOCKCYCLES  1
#define I2C_FILTER_2CLOCKCYCLES  2
#define I2C_FILTER_15CLOCKCYCLES 15

typedef struct
{
  uint32_t ClockSpeed; /* Задает тактовую частоту.
                          Этот параметр должен быть меньше чем 400kHz */

  uint32_t AddressingMode; /* Задает режим адреса 7-битный или 10-битный.
                              Этот параметр может быть одним из значений I2C_addressing_mode */

  uint32_t OwnAddress1; /* Задает собственный аддрес.
                           Этот параметр долен быть 7-битным или 10-битным адресом. */

  uint32_t DualAddressMode; /* Задает режим дополнительного адреса 7 бит.
                               этот параметр может быть одним из значений I2C_dual_addressing_mode */

  uint32_t OwnAddress2; /* Задает второй собственный адрес, если выбран режим дополнительного адреса
                           этот параметр должен быть 7-битным адресом. */

  uint32_t OwnAddress2Mask; /* Задает маску второго собственного адреса
                               этот параметр должен быть одним из значений I2C_OwnAddress2_mask. */

  uint32_t NoStretchMode; /* Задает режим удержания SCL ведомого.
                             Этот параметр может быть одним из значений I2C_nostretch_mode */

  uint32_t SBCMode; /* Задает режим аппаратного контроля байта ведомым.
                       Этот параметр может быть одним из значений I2C_SBC_mode */

  uint32_t ReloadMode; /* Задает режим перезагрузки.
                          Этот параметр может быть одним из значений I2C_reload_mode */

  uint32_t AutoEnd; /* Задает режим аппаратного контроля байта ведомым.
                    Этот параметр может быть одним из значений I2C_autoend_mode */

  uint32_t Filter; /* Управление цифровым фильтром.
                    Этот параметр может быть одним из значений I2C_filter */

} I2C_InitTypeDef;

typedef struct
{
  uint32_t PRESC; /* Делитель частоты I2CCLK. 
                    Этот параметр может быть от 1 до 15 */
              
  uint32_t SCLDEL; /* Задержка между изменением SDA и фронтом SCL в режиме 
                     ведущего и ведомого при NOSTRETCH = 0 
                     Этот параметр может быть от 1 до 15 */

  uint32_t SDADEL; /* Задержка между спадом SCL и изменением SDA в режиме
                     ведущего и ведомого при NOSTRETCH = 0
                     Этот параметр может быть от 1 до 15 */

  uint32_t SCLH; /* Время удержания SCL в состоянии логической «1» в режиме ведущего
                   Этот параметр может быть от 1 до 255 */

  uint32_t SCLL; /* Время удержания SCL в состоянии логического «0» в режиме ведущего
                   Этот параметр может быть от 1 до 255 */

} I2C_ClockTypeDef;

typedef enum {
  HAL_I2C_MODE_MASTER = 0, /* Режим ведущего */
  HAL_I2C_MODE_SLAVE = 1,  /* Режим ведомого */

} HAL_I2C_ModeTypeDef;

typedef struct
{
  I2C_TypeDef *Instance; /* Базоый адрес регистров I2C */

  I2C_InitTypeDef Init;      /* Параметры I2C */
  I2C_ClockTypeDef Clock;    /* Параметры частоты I2C */
  uint8_t *pBuff;            /* Указатель на буфер передачи */
  uint32_t TransferSize;     /* Количество байт при передаче данных */
  uint8_t TransferDirection; /* Направление передачи */

  HAL_I2C_ModeTypeDef Mode; /* Режим I2C */
  uint32_t ErrorCode;       /* Код ошибки i2C */
  uint16_t SlaveAddress;    /* Адрес ведомого устройства */
  uint8_t ShiftAddress;     /* Сдвиг адреса ведомого на 1 бит влево
                               Этот параметр может быть одним из значений I2C_address_shift */

} I2C_HandleTypeDef;

void HAL_I2C_Disable(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Enable(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ReloadDisable(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ReloadEnable(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SetFilter(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SetClockSpeed(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
void HAL_I2C_CheckError(I2C_HandleTypeDef *hi2c);

/* Ведущий */
void HAL_I2C_Master_Stop(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Master_Restart(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Master_CheckError(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Master_Transfer_Init(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Master_WriteNBYTE(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count);
void HAL_I2C_Master_ReadNBYTE(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count);
void HAL_I2C_Master_Write(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count);
void HAL_I2C_Master_Read(I2C_HandleTypeDef *hi2c, uint16_t slave_adr, uint8_t data[], uint32_t byte_count);

/* Ведомый */
extern void HAL_I2C_Slave_SBC(I2C_HandleTypeDef *hi2c, uint32_t byte_count);
void HAL_I2C_Slave_WaitADDR(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Slave_ACK(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Slave_NACK(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Slave_CleanFlag(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Slave_Restart(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Slave_CheckError(I2C_HandleTypeDef *hi2c);
void HAL_I2C_Slave_WriteNBYTE(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count);
void HAL_I2C_Slave_ReadNBYTE(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count);
void HAL_I2C_Slave_Write(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count);
void HAL_I2C_Slave_Read(I2C_HandleTypeDef *hi2c, uint8_t data[], uint32_t byte_count);

#endif