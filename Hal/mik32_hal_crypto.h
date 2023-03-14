#ifndef MIK32_HAL_CRYPTO
#define MIK32_HAL_CRYPTO

#include "def_list.h"
#include "mcu32_memory_map.h"
#include "crypto.h"
#include "pad_config.h"

#ifdef MIK32_CRYPTO_DEBUG
#include "common.h"
#endif


/* Title: Макросы */

/*
 * Defines: Длина вектора инициализации 
 *
 * IV_LENGTH_KUZNECHIK_CBC - Количество слов вектора инициализации в режиме шифрования CBC (Кузнечик)
 * IV_LENGTH_MAGMA_CBC - Количество слов вектора инициализации в режиме шифрования CBC (Магма) 
 * IV_LENGTH_AES_CBC - Количество слов вектора инициализации в режиме шифрования CBC (AES)
 * IV_LENGTH_KUZNECHIK_CTR - Количество слов вектора инициализации в режиме шифрования CTR (Кузнечик)
 * IV_LENGTH_MAGMA_CTR - Количество слов вектора инициализации в режиме шифрования CTR (Магма)
 * IV_LENGTH_AES_CTR - Количество слов вектора инициализации в режиме шифрования CTR (AES)
 *
 */
#define IV_LENGTH_KUZNECHIK_CBC       4       /* Количество слов вектора инициализации в режиме шифрования CBC (Кузнечик) */
#define IV_LENGTH_MAGMA_CBC           4       /* Количество слов вектора инициализации в режиме шифрования CBC (Магма) */
#define IV_LENGTH_AES_CBC             2       /* Количество слов вектора инициализации в режиме шифрования CBC (AES) */
#define IV_LENGTH_KUZNECHIK_CTR       2       /* Количество слов вектора инициализации в режиме шифрования CTR (Кузнечик) */
#define IV_LENGTH_MAGMA_CTR           2       /* Количество слов вектора инициализации в режиме шифрования CTR (Магма) */
#define IV_LENGTH_AES_CTR             1       /* Количество слов вектора инициализации в режиме шифрования CTR (AES) */

/*
 * Defines: Длина ключа
 *
 * IV_LENGTH - Длина вектора инииалазиции  
 * CRYPTO_KEY_KUZNECHIK - Длина ключа Кузнечик
 * CRYPTO_KEY_MAGMA - Длина ключа Магма
 * CRYPTO_KEY_AES - Длина ключа AES
 *
 */
#define CRYPTO_KEY_KUZNECHIK        8       /* Длина ключа Кузнечик */
#define CRYPTO_KEY_MAGMA            8       /* Длина ключа Магма */
#define CRYPTO_KEY_AES              4       /* Длина ключа AES */

/*
 * Defines: Длина блока
 *
 * CRYPTO_BLOCK_KUZNECHIK - Длина блока Кузнечик
 * CRYPTO_BLOCK_MAGMA - Длина блока Магма
 * CRYPTO_BLOCK_AES - Длина блока AES
 *
 */
#define CRYPTO_BLOCK_KUZNECHIK        4       /* Длина блока Кузнечик */
#define CRYPTO_BLOCK_MAGMA            2       /* Длина блока Магма */
#define CRYPTO_BLOCK_AES              4       /* Длина блока AES */

/*
 * Defines: Алгоритм шифрования
 *
 * CRYPTO_ALG_KUZNECHIK - Алгоритм шифрования Кузнечик
 * CRYPTO_ALG_MAGMA - Алгоритм шифрования Магма  
 * CRYPTO_ALG_AES - Алгоритм шифрования AES
 *
 */
#define CRYPTO_ALG_KUZNECHIK        0       /* Алгоритм шифрования Кузнечик */
#define CRYPTO_ALG_MAGMA            1       /* Алгоритм шифрования Магма */
#define CRYPTO_ALG_AES              2       /* Алгоритм шифрования AES */

/*
 * Defines: Режим шифрования
 *
 * CRYPTO_CIPHER_MODE_ECB - Режим шифрования ECB (Electronic Codebook)
 * CRYPTO_CIPHER_MODE_CBC - Режим шифрования CBC (Cipher Block Chaining)
 * CRYPTO_CIPHER_MODE_CTR - Режим шифрования CTR (Counter mode)
 *
 */
#define CRYPTO_CIPHER_MODE_ECB       0       /* Режим шифрования ECB (Electronic Codebook) */
#define CRYPTO_CIPHER_MODE_CBC       1       /* Режим шифрования CBC (Cipher Block Chaining) */
#define CRYPTO_CIPHER_MODE_CTR       2       /* Режим шифрования CTR (Counter mode) */

/*
 * Defines: Перестановка слова
 *
 * CRYPTO_SWAP_MODE_NONE - Нет перестановки
 * CRYPTO_SWAP_MODE_HALFWORD - Перестановка по полуслову
 * CRYPTO_SWAP_MODE_BYTE - Перестановки по байтам
 * CRYPTO_SWAP_MODE_BIT - Перестановка по битам
 *
 */
#define CRYPTO_SWAP_MODE_NONE        0       /* Нет перестановки */
#define CRYPTO_SWAP_MODE_HALFWORD    1       /* Перестановка по полуслову */
#define CRYPTO_SWAP_MODE_BYTE        2       /* Перестановки по байтам */
#define CRYPTO_SWAP_MODE_BIT         3       /* Перестановка по битам */

/*
 * Defines: Порядок загрузки/выгрузки
 *
 * CRYPTO_ORDER_MODE_LSW - Порядка загрузки/выгрузки от младшего слова к старшему
 * CRYPTO_ORDER_MODE_MSW - Порядка загрузки/выгрузки от старшего слова к младшему
 *
 */
#define CRYPTO_ORDER_MODE_LSW        0       /* Порядка загрузки/выгрузки от младшего слова к старшему */
#define CRYPTO_ORDER_MODE_MSW        1       /* Порядка загрузки/выгрузки от старшего слова к младшему */

/*
 * Defines: Максимальная длина ключа
 *
 * MAXIMUM_KEY_LENGTH - Максимальная длина ключа
 *
 */
#define MAXIMUM_KEY_LENGTH      8       /* Максимальная длина ключа */  

/* Title: Структуры */

/*
 * Struct: Crypto_HandleTypeDef
 * Настройки Crypto
 * 
 */
typedef struct
{
    /*
    * Variable: Instance
    * Базоый адрес регистров Crypto
    *
    */
    CRYPTO_TypeDef *Instance;

    /*
    * Variable: Algorithm
    * Выбор алгоритма шифрования
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <CRYPTO_ALG_KUZNECHIK>;
    * - <CRYPTO_ALG_MAGMA>;
    * - <CRYPTO_ALG_AES>.  
    *
    */
    uint8_t Algorithm;

    /*
    * Variable: CipherMode
    * Выбор режима шифрования
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <CRYPTO_CIPHER_MODE_ECB>;
    * - <CRYPTO_CIPHER_MODE_CBC>;
    * - <CRYPTO_CIPHER_MODE_CTR>.
    *
    */
    uint8_t CipherMode;

    /*
    * Variable: SwapMode
    * Перестановка слова
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <CRYPTO_SWAP_MODE_NONE>;
    * - <CRYPTO_SWAP_MODE_HALFWORD>;
    * - <CRYPTO_SWAP_MODE_BYTE>;
    * - <CRYPTO_SWAP_MODE_BIT>.
    *
    */
    uint8_t SwapMode;

    /*
    * Variable: OrderMode
    * Выбор порядка загрузки/выгрузки
    * 
    * Этот параметр должен быть одним из значений:
    * 
    * - <CRYPTO_ORDER_MODE_LSW>;
    * - <CRYPTO_ORDER_MODE_MSW>.
    *
    */
    uint8_t OrderMode;

} Crypto_HandleTypeDef;

/* Title: Функции */

/*
 * Function: HAL_Crypto_CounterReset
 * Сброс счётчиков загружаемых/выгружаемых данных
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 *
 * Returns:
 * void
 */
void HAL_Crypto_CounterReset(Crypto_HandleTypeDef *hcrypto);

/*
 * Function: HAL_Crypto_WaitReady
 * Ожидать когда модуль станет доступен
 * 
 * Модуль доступен если флаг Ready = 1
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 *
 * Returns:
 * void
 */
void HAL_Crypto_WaitReady(Crypto_HandleTypeDef *hcrypto);

/*
 * Function: HAL_Crypto_SetAlgorithm
 * Задать алгоритм шифрования
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * Algorithm - Алгоритм шифрования
 *
 * Returns:
 * void
 */
void HAL_Crypto_SetAlgorithm(Crypto_HandleTypeDef *hcrypto, uint8_t Algorithm);

/*
 * Function: HAL_Crypto_SetCipherMode
 * Задать режим шифрования
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * CipherMode - Режим шифрования
 *
 * Returns:
 * void
 */
void HAL_Crypto_SetCipherMode(Crypto_HandleTypeDef *hcrypto, uint8_t CipherMode);

/*
 * Function: HAL_Crypto_SetSwapMode
 * Задать режим перестановки слова
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * SwapMode - Режим перестановки слова
 *
 * Returns:
 * void
 */
void HAL_Crypto_SetSwapMode(Crypto_HandleTypeDef *hcrypto, uint8_t SwapMode);

/*
 * Function: HAL_Crypto_SetOrderMode
 * Задать порядок загрузки/выгрузки данных
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * OrderMode - Порядок загрузки/выгрузки данных
 *
 * Returns:
 * void
 */
void HAL_Crypto_SetOrderMode(Crypto_HandleTypeDef *hcrypto, uint8_t OrderMode);

/*
 * Function: HAL_Crypto_SetIV
 * Задать вектор инициализации 
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * InitVector - Вектор инициализации (IV)
 * IvLength - Количество слов в InitVector
 *
 * Returns:
 * void
 */
void HAL_Crypto_SetIV(Crypto_HandleTypeDef *hcrypto, uint32_t InitVector[], uint32_t IvLength);

/*
 * Function: HAL_Crypto_SetKey
 * Задать мастер-ключ
 * 
 * Ключ должен быть инициализирован в режиме шифрования (CONFIG.decode = 0).
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * crypto_key - Ключ  
 *
 * Returns:
 * void
 */
void HAL_Crypto_SetKey(Crypto_HandleTypeDef *hcrypto, uint32_t crypto_key[]);

/*
 * Function: HAL_Crypto_Init
 * Инициализировать Crypto в соответствии с настройками <Crypto_HandleTypeDef> *hcrypto
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 *
 * Returns:
 * void
 */
void HAL_Crypto_Init(Crypto_HandleTypeDef *hcrypto);

/*
 * Function: HAL_Crypto_Encode
 * Зашифровать текст
 * 
 * Зашифрованный текст передается в массив cipher_text
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * plain_text - Массив с данными незашифрованного текста
 * cipher_text - Массив с данными для зашифрованного текста
 * text_length - Количество слов в тексте
 *
 * Returns:
 * void
 */
void HAL_Crypto_Encode(Crypto_HandleTypeDef *hcrypto, uint32_t plain_text[], uint32_t cipher_text[], uint32_t text_length);

/*
 * Function: HAL_Crypto_Encode
 * Расшифровать текст
 * 
 * Расшифрованный текст передаетсяв массив plain_text
 *
 * Parameters:
 * hcrypto - Указатель на структуру с настройками Crypto
 * cipher_text - Массив с данными зашифрованного текста
 * plain_text - Массив с данными для незашифрованного текста
 * text_length - Количество слов в тексте
 *
 * Returns:
 * void
 */
void HAL_Crypto_Decode(Crypto_HandleTypeDef *hcrypto, uint32_t cipher_text[], uint32_t plain_text[], uint32_t text_length);

#endif