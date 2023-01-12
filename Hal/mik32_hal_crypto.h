#ifndef MIK32_HAL_CRYPTO
#define MIK32_HAL_CRYPTO

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "crypto.h"
#include "pad_config.h"

#ifdef MIK32_CRYPTO_DEBUG
#include "common.h"
#endif


/* Длина вектора инииалазиции */
#define IV_LENGTH       4

/* Длина ключа */
#define CRYPTO_KEY_KUZNECHIK        8       /* Длина ключа Кузнечик */
#define CRYPTO_KEY_MAGMA            8       /* Длина ключа Магма */
#define CRYPTO_KEY_AES              4       /* Длина ключа AES */

/* Длина блока */
#define CRYPTO_BLOCK_KUZNECHIK        4       /* Длина блока Кузнечик */
#define CRYPTO_BLOCK_MAGMA            2       /* Длина блока Магма */
#define CRYPTO_BLOCK_AES              4       /* Длина блока AES */

/* Выбор алгоритма шифрования */
#define CRYPTO_ALG_KUZNECHIK        0       /* Алгоритм шифрования Кузнечик */
#define CRYPTO_ALG_MAGMA            1       /* Алгоритм шифрования Магма */
#define CRYPTO_ALG_AES              2       /* Алгоритм шифрования AES */

/* Выбор режима шифрования */
#define CRYPTO_CIPHER_MODE_ECB       0       /* Режим шифрования ECB (Electronic Codebook) */
#define CRYPTO_CIPHER_MODE_CBC       1       /* Режим шифрования CBC (Cipher Block Chaining) */
#define CRYPTO_CIPHER_MODE_CTR       2       /* Режим шифрования CTR (Counter mode) */

/* Перестановка слова */
#define CRYPTO_SWAP_MODE_NONE        0       /* Нет перестановки */
#define CRYPTO_SWAP_MODE_HALFWORD    1       /* Перестановка по полуслову */
#define CRYPTO_SWAP_MODE_BYTE        2       /* Перестановки по байтам */
#define CRYPTO_SWAP_MODE_BIT         3       /* Перестановка по битам */

/* Выбор порядка загрузки/выгрузки */
#define CRYPTO_ORDER_MODE_LSW        0       /* Порядка загрузки/выгрузки от младшего слова к старшему */
#define CRYPTO_ORDER_MODE_MSW        1       /* Порядка загрузки/выгрузки от старшего слова к младшему */

#define MAXIMUM_KEY_LENGTH      8       /* Максимальная длина ключа */  


typedef struct
{
    CRYPTO_TypeDef *Instance;           /* Базоый адрес регистров ANALOG_REG */

    uint8_t Algorithm;                  /* Выбор алгоритма шифрования */ 

    uint8_t CipherMode;                 /* Выбор режима шифрования */

    uint8_t SwapMode;                   /* Перестановка слова */

    uint8_t OrderMode;                  /* Выбор порядка загрузки/выгрузки */

} Crypto_HandleTypeDef;


void HAL_Crypto_CounterReset(Crypto_HandleTypeDef *hcrypto);
void HAL_Crypto_WaitReady(Crypto_HandleTypeDef *hcrypto);
void HAL_Crypto_SetAlgorithm(Crypto_HandleTypeDef *hcrypto, uint8_t Algorithm);
void HAL_Crypto_SetCipherMode(Crypto_HandleTypeDef *hcrypto, uint8_t CipherMode);
void HAL_Crypto_SetSwapMode(Crypto_HandleTypeDef *hcrypto, uint8_t SwapMode);
void HAL_Crypto_SetOrderMode(Crypto_HandleTypeDef *hcrypto, uint8_t OrderMode);
void HAL_Crypto_SetINIT(Crypto_HandleTypeDef *hcrypto, uint32_t InitVector[]);
void HAL_Crypto_SetKey(Crypto_HandleTypeDef *hcrypto, uint32_t crypto_key[]);
void HAL_Crypto_Init(Crypto_HandleTypeDef *hcrypto);
void HAL_Crypto_Encode(Crypto_HandleTypeDef *hcrypto, uint32_t plain_text[], uint32_t cipher_text[], uint32_t text_length);
void HAL_Crypto_Decode(Crypto_HandleTypeDef *hcrypto, uint32_t cipher_text[], uint32_t plain_text[], uint32_t text_length);

#endif