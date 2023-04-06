#include "uart_lib.h"
#include "xprintf.h"

#include "mik32_hal_rcc.h"
#include "mik32_hal_crypto.h"
#include "mik32_hal_dma.h"

Crypto_HandleTypeDef hcrypto;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
DMA_ChannelHandleTypeDef hdma_ch1;

void SystemClock_Config(void);
static void Crypto_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef* hdma);
static void DMA_CH1_Init(DMA_InitTypeDef* hdma);
static void DMA_Init(void);

uint32_t crypto_key[CRYPTO_KEY_KUZNECHIK] = {0x8899aabb, 0xccddeeff, 0x00112233, 0x44556677, 0xfedcba98, 0x76543210, 0x01234567, 0x89abcdef};
                    

void dma_kuznechik_ECB_code()
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

    // for (uint32_t i = 0; i < sizeof(plain_text)/sizeof(*plain_text); i += CRYPTO_BLOCK_KUZNECHIK)
    // {
    //     /* Зашифровать данные */
    //     HAL_DMA_Start(&hdma_ch0, plain_text+i, (void*)&hcrypto.Instance->BLOCK, CRYPTO_BLOCK_KUZNECHIK*4 - 1);
    //     HAL_DMA_Wait(&hdma_ch0); 
    //     // HAL_Crypto_WaitReady(&hcrypto);
    //     /* Получить зашифрованные данные */
    //     HAL_DMA_Start(&hdma_ch1, (void*)&hcrypto.Instance->BLOCK, cipher_text+i, CRYPTO_BLOCK_KUZNECHIK*4 - 1);
    //     HAL_DMA_Wait(&hdma_ch1);
    // }


    /* Зашифровать данные */
    HAL_DMA_Start(&hdma_ch0, plain_text, (void*)&hcrypto.Instance->BLOCK, plain_text_length*4 - 1);
    HAL_DMA_Start(&hdma_ch1, (void*)&hcrypto.Instance->BLOCK, cipher_text, plain_text_length*4 - 1);
    HAL_DMA_Wait(&hdma_ch0); 
    HAL_DMA_Wait(&hdma_ch1); 


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



int main()
{    
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Crypto_Init();

    DMA_Init();


    xprintf("\nkuznechik_ECB_code\n");
    dma_kuznechik_ECB_code();
    
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

    PeriphClkInit.PMClockAHB = PMCLOCKAHB_DEFAULT | PM_CLOCK_CRYPTO_M | PM_CLOCK_DMA_M;    
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


static void DMA_CH0_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch0.Instance = hdma->Instance;
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;  
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD;       /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_CRYPTO_REQUEST;
    hdma_ch1.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD;     /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_CRYPTO_REQUEST;
    hdma_ch1.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;  

    HAL_DMA_ChannelInit(&hdma_ch0);
}

static void DMA_CH1_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch1.Instance = hdma->Instance;
    hdma_ch1.ChannelInit.Channel = DMA_CHANNEL_1;  
    hdma_ch1.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch1.ChannelInit.ReadMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch1.ChannelInit.ReadInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch1.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD;       /* data_len должно быть кратно read_size */
    hdma_ch1.ChannelInit.ReadBurstSize = 2;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch1.ChannelInit.ReadRequest = DMA_CHANNEL_CRYPTO_REQUEST;
    hdma_ch1.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch1.ChannelInit.WriteMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch1.ChannelInit.WriteInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch1.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD;     /* data_len должно быть кратно write_size */
    hdma_ch1.ChannelInit.WriteBurstSize = 2;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch1.ChannelInit.WriteRequest = DMA_CHANNEL_CRYPTO_REQUEST;  
    hdma_ch1.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;

    HAL_DMA_ChannelInit(&hdma_ch1);
}


static void DMA_Init(void)
{
    
    /* Настройки DMA */
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_ENABLED;
    HAL_DMA_Init(&hdma);

    /* Инициализация канала */
    DMA_CH0_Init(&hdma);
    DMA_CH1_Init(&hdma);

}








