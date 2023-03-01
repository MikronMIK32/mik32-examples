#include "main.h"

Crypto_HandleTypeDef hcrypto;

void SystemClock_Config(void);
static void Crypto_Init(void);

uint32_t crypto_key[CRYPTO_KEY_KUZNECHIK] = {0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0xfedcba98, 0x76543210, 0x01234567, 0x89abcdef};


void kuznechik_ECB_code()
{
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };

                    

    uint32_t expect_cipher_text[] = {
                                        0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                        0xb429912c, 0x6e0032f9, 0x285452d7, 0x6718d08b, 
                                        0xf0ca3354, 0x9d247cee, 0xf3f5a531, 0x3bd4b157, 
                                        0xd0b09ccd, 0xe830b9eb, 0x3a02c4c5, 0xaa8ada98
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);

    uint32_t cipher_text[plain_text_length];

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_ECB);

    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);

    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("plain: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");   

    xprintf("cipher: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", expect_cipher_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        if(expect_cipher_text[i] != cipher_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == plain_text_length)
        {
            xprintf("Successfull\n");
        }
    }
    xprintf("\n");
}

void kuznechik_ECB_decode()
{
    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };

                
    uint32_t cipher_text[] = {  
                                0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                0xb429912c, 0x6e0032f9, 0x285452d7, 0x6718d08b, 
                                0xf0ca3354, 0x9d247cee, 0xf3f5a531, 0x3bd4b157, 
                                0xd0b09ccd, 0xe830b9eb, 0x3a02c4c5, 0xaa8ada98
                             };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);

    uint32_t plain_text[cipher_text_length];

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_ECB);

    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);
    
    /* Расшифровать данные */
    HAL_Crypto_Decode(&hcrypto, cipher_text, plain_text, cipher_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("cipher: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");   

    xprintf("plain: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", expect_plain_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        if(expect_plain_text[i] != plain_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == cipher_text_length)
        {
            xprintf("Successfull\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CBC_code()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CBC] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
    
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };

                    

    uint32_t expect_cipher_text[] = {
                                        0xd4643dbb, 0x6b9501a7, 0xb613a7a4, 0x8652e5e9, 
                                        0xbbba9af5, 0x1e7bc4dd, 0xe1e9a0f2, 0xb7c2155b, 
                                        0x1287d133, 0x5b1c0eee, 0x29387b0d, 0xdc4d481b, 
                                        0x86486ab6, 0xc8bce113, 0x69c607a8, 0xe3f91801
                                    };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);

    uint32_t cipher_text[plain_text_length];

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CBC);

    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector));  
    
    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length);     

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("plain: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");   

    xprintf("cipher: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", expect_cipher_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        if(expect_cipher_text[i] != cipher_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == plain_text_length)
        {
            xprintf("Successfull\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CBC_decode()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CBC] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
    
    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };

                
    uint32_t cipher_text[] = {  
                                0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                0x1ac9d976, 0xf83636f5, 0x5ae9ef30, 0x5e7c90d2, 
                                0x15645af4, 0xa78e50a9, 0xabe8db4b, 0x754de3f2, 
                                0x9cda9458, 0x8e61dc9c, 0xe42e2643, 0x75303df2
                             };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);

    uint32_t plain_text[cipher_text_length];

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CBC);
    
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector));  

    /* Расшифровать данные */
    HAL_Crypto_Decode(&hcrypto, cipher_text, plain_text, cipher_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("cipher: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");   

    xprintf("plain: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", expect_plain_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        if(expect_plain_text[i] != plain_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == cipher_text_length)
        {
            xprintf("Successfull\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CTR_code()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CTR] = {0x00000000, 0x00000000};
    
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };

                    

    uint32_t expect_cipher_text[] = {
                                        0x675b2a7e, 0xf0b9d788, 0xbb9e7619, 0x013c144b, 
                                        0x8154a662, 0xf7c93530, 0x58d02698, 0xb431e88a, 
                                        0x02525e18, 0x62fa6cd9, 0xa9105a1a, 0x8fb9b742, 
                                        0x36d4762c, 0xcffb8e64, 0xd3765c2a, 0x6fb7f734
                                    };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);

    uint32_t cipher_text[plain_text_length];

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CTR);
    
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector));   

    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length); 


    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("plain: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");   

    xprintf("cipher: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        xprintf("0x%08x, ", expect_cipher_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < plain_text_length; i++)
    {
        if(expect_cipher_text[i] != cipher_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == plain_text_length)
        {
            xprintf("Successfull\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CTR_decode()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CTR] = {0x00000000, 0x00000000};
    
    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };

                
    uint32_t cipher_text[] = {  
                                0x859cf21a, 0x73fa86e5, 0xf91ef655, 0xf7a01728, 
                                0x01c95134, 0x7e94a413, 0xdda109d0, 0xff8009cd, 
                                0xa52e6661, 0xa7f21c84, 0x26c26636, 0x50f5d618, 
                                0xdaa7da93, 0x012565b2, 0x02d41642, 0x6f31b25c
                             };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);

    uint32_t plain_text[cipher_text_length];

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CTR);
    
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector));  

    /* Расшифровать данные */
    HAL_Crypto_Decode(&hcrypto, cipher_text, plain_text, cipher_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
    }
    xprintf("\n");  

    xprintf("cipher: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", cipher_text[i]);
    }
    xprintf("\n");   

    xprintf("plain: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", plain_text[i]);
    }
    xprintf("\n");

    xprintf("expect: ");
    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        xprintf("0x%08x, ", expect_plain_text[i]);
    }
    xprintf("\n");

    for (uint32_t i = 0; i < cipher_text_length; i++)
    {
        if(expect_plain_text[i] != plain_text[i])
        {
            xprintf("error\n");
            break;
        }
        else if ((i+1) == cipher_text_length)
        {
            xprintf("Successfull\n");
        }
    }
    xprintf("\n");
}

int main()
{    

    SystemClock_Config();

    Crypto_Init();

      
    xprintf("\nkuznechik_ECB_code\n");
    kuznechik_ECB_code();
    xprintf("\nkuznechik_ECB_decode\n");   
    kuznechik_ECB_decode();


    xprintf("\nkuznechik_CBC_code\n");
    kuznechik_CBC_code();
    xprintf("\nkuznechik_CBC_decode\n");   
    kuznechik_CBC_decode();


    xprintf("\nkuznechik_CTR_code\n");
    kuznechik_CTR_code();
    xprintf("\nkuznechik_CTR_decode\n");   
    kuznechik_CTR_decode();


    while (1)
    {    
        
    }
       
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInit.OscillatorEnable = RCC_OSCILLATORTYPE_OSC32K | RCC_OSCILLATORTYPE_OSC32M;   
    RCC_OscInit.OscillatorSystem = RCC_OSCILLATORTYPE_OSC32M;                          
    RCC_OscInit.AHBDivider = 0;                             
    RCC_OscInit.APBMDivider = 0;                             
    RCC_OscInit.APBPDivider = 0;                             
    RCC_OscInit.HSI32MCalibrationValue = 0;                  
    RCC_OscInit.LSI32KCalibrationValue = 0;
    HAL_RCC_OscConfig(&RCC_OscInit);

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT | PM_CLOCK_CRYPTO_M;    
    PeriphClkInit.PMClockAPB_M = PMCLOCKAPB_M_DEFAULT | PM_CLOCK_WU_M | PM_CLOCK_PAD_CONFIG_M;     
    PeriphClkInit.PMClockAPB_P = PMCLOCKAPB_P_DEFAULT | PM_CLOCK_UART_0_M; 
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_NO_CLK;
    PeriphClkInit.RTCClockCPUSelection = RCC_RTCCLKCPUSOURCE_NO_CLK;
    HAL_RCC_ClockConfig(&PeriphClkInit);
}

static void Crypto_Init(void)
{
    hcrypto.Instance = CRYPTO;

    hcrypto.Algorithm = CRYPTO_ALG_KUZNECHIK;
    hcrypto.CipherMode = CRYPTO_CIPHER_MODE_ECB;
    hcrypto.SwapMode = CRYPTO_SWAP_MODE_NONE; 
    hcrypto.OrderMode = CRYPTO_ORDER_MODE_MSW;

    HAL_Crypto_Init(&hcrypto);
}
