#include "mik32_hal_usart.h"
#include "xprintf.h"

#include "mik32_hal_pcc.h"
#include "mik32_hal_crypto.h"

/*
* В примере демонстрируется работа с крипто-блоком
*
* Используется алгоритм шифрования Кузнечик в разных режимах шифрования (ECB, CBC, CTR)
* Ключ и открытый текст взяты из контрольных примеров ГОСТ 34.13—2015 приложение А.
*
* В каждом режиме шифрования текст зашифровывается, затем расшифровывается и сравнивается с ожидаемым. Результат выводится в UART0.
*/


Crypto_HandleTypeDef hcrypto;

USART_HandleTypeDef husart0;

void SystemClock_Config(void);
static void Crypto_Init(void);
void USART_Init();

uint32_t crypto_key[CRYPTO_KEY_KUZNECHIK] = {0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0xfedcba98, 0x76543210, 0x01234567, 0x89abcdef};

                           

void kuznechik_ECB_code()
{
    uint32_t plain_text[] =     {            
                                    0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                    0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                    0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                    0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                };
    
    uint32_t cipher_text[] = {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 

    uint32_t expect_cipher_text[] = {
                                        0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                        0xb429912c, 0x6e0032f9, 0x285452d7, 0x6718d08b, 
                                        0xf0ca3354, 0x9d247cee, 0xf3f5a531, 0x3bd4b157, 
                                        0xd0b09ccd, 0xe830b9eb, 0x3a02c4c5, 0xaa8ada98
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_ECB);
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);


    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length); 


    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
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
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_ECB_decode()
{
    uint32_t plain_text[] =     {            
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0
                                };
    
    uint32_t cipher_text[] = {  
                                0x7f679d90, 0xbebc2430, 0x5a468d42, 0xb9d4edcd, 
                                0xb429912c, 0x6e0032f9, 0x285452d7, 0x6718d08b, 
                                0xf0ca3354, 0x9d247cee, 0xf3f5a531, 0x3bd4b157, 
                                0xd0b09ccd, 0xe830b9eb, 0x3a02c4c5, 0xaa8ada98
                             }; 
    
    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);


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
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CBC_code()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CBC] = {0x12341234, 0x11114444, 0xABCDABCD, 0xAAAABBBB};
    
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };

    uint32_t cipher_text[] = {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 

    uint32_t expect_cipher_text[] = {
                                        0x50796e7f, 0x4094ce10, 0xbab7374c, 0x981047e3, 
                                        0x1ee4f83b, 0x334948ed, 0x86a0873c, 0x86bff9a2, 
                                        0xa084f5fa, 0x965481e4, 0xb64be9bd, 0x32ef21e3, 
                                        0xa6e376cf, 0x95e8a097, 0x9a46ba33, 0x152b1843
                                    };

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CBC);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);


    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length);     

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
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
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CBC_decode()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CBC] = {0x12341234, 0x11114444, 0xABCDABCD, 0xAAAABBBB};
    
    uint32_t plain_text[] =     {            
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0, 
                                    0x0, 0x0, 0x0, 0x0
                                };
    
    uint32_t cipher_text[] = {  
                                0x50796e7f, 0x4094ce10, 0xbab7374c, 0x981047e3, 
                                0x1ee4f83b, 0x334948ed, 0x86a0873c, 0x86bff9a2, 
                                0xa084f5fa, 0x965481e4, 0xb64be9bd, 0x32ef21e3, 
                                0xa6e376cf, 0x95e8a097, 0x9a46ba33, 0x152b1843
                             }; 

    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };
    

    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text);

    

    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CBC);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
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
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

void kuznechik_CTR_code()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CTR] = {0x12345678, 0x90ABCEF0};
    
    uint32_t plain_text[] = {            
                                0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                            };
    
    uint32_t cipher_text[] = {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 

    uint32_t expect_cipher_text[] = {
                                        0xf195d8be, 0xc10ed1db, 0xd57b5fa2, 0x40bda1b8,
                                        0x85eee733, 0xf6a13e5d, 0xf33ce4b3, 0x3c45dee4,
                                        0xa5eae88b, 0xe6356ed3, 0xd5e877f1, 0x3564a3a5,
                                        0xcb91fab1, 0xf20cbab6, 0xd1c6d158, 0x20bdba73
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t plain_text_length = sizeof(plain_text)/sizeof(*plain_text);


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CTR);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
    /* Установка ключа */
    HAL_Crypto_SetKey(&hcrypto, crypto_key);

    /* Зашифровать данные */
    HAL_Crypto_Encode(&hcrypto, plain_text, cipher_text, plain_text_length); 

    xprintf("KEY ");
    for (uint32_t i = 0; i < key_length; i++)
    {
        xprintf("0x%08x, ", crypto_key[i]);
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
            xprintf("Success\n"); 
        }
    }
    xprintf("\n");
}

void kuznechik_CTR_decode()
{
    uint32_t init_vector[IV_LENGTH_KUZNECHIK_CTR] = {0x12345678, 0x90ABCEF0};
    
    uint32_t plain_text[] =  {  
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0
                             }; 
    
    uint32_t cipher_text[] = {
                                0xf195d8be, 0xc10ed1db, 0xd57b5fa2, 0x40bda1b8, 
                                0x85eee733, 0xf6a13e5d, 0xf33ce4b3, 0x3c45dee4,
                                0xa5eae88b, 0xe6356ed3, 0xd5e877f1, 0x3564a3a5
                             };

    uint32_t expect_plain_text[] =  {            
                                        0x11223344, 0x55667700, 0xffeeddcc, 0xbbaa9988,
                                        0x00112233, 0x44556677, 0x8899aabb, 0xcceeff0a,
                                        0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0a00,
                                        0x22334455, 0x66778899, 0xaabbccee, 0xff0a0011
                                    };


    uint32_t key_length = sizeof(crypto_key)/sizeof(*crypto_key);
    uint32_t cipher_text_length = sizeof(cipher_text)/sizeof(*cipher_text); 


    /* Задать режим шифрования */
    HAL_Crypto_SetCipherMode(&hcrypto, CRYPTO_CIPHER_MODE_CTR);
    /* Установка вектора инициализации */  
    HAL_Crypto_SetIV(&hcrypto, init_vector, sizeof(init_vector)/sizeof(*init_vector)); 
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
            xprintf("Success\n");
        }
    }
    xprintf("\n");
}

int main()
{    
    SystemClock_Config();

    USART_Init();

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
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
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


void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Disable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode3;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    husart0.dma_tx_request = Disable;
    husart0.dma_rx_request = Disable;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable; //out
    husart0.Modem.cts = Disable; //in
    husart0.Modem.dtr = Disable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Disable;//out
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}
