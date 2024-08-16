#include "mik32_hal_crypto.h"
#include "mik32_hal_dma.h"

#include "mik32_hal_usart.h"
#include "xprintf.h"

/*
* В данном примере демонстрируется работа DMA с крипто-блоком.
*
* Канал 0 DMA передает массив данных plain_text в крипто-блок, а канал 1 записывает зашифрованные данные в массив cipher_text.
* Затем зашифрованные данные сравниваются с ожидаемыми данными. 
* 
* Результат выводится по UART0.
* При шифровании используется алгоритм "Кузнечик" в режиме шифрования ECB.
* Ключ и открытый текст взяты из контрольных примеров ГОСТ 34.13—2015 приложение А.
*/

Crypto_HandleTypeDef hcrypto;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
DMA_ChannelHandleTypeDef hdma_ch1;
USART_HandleTypeDef husart0;

void SystemClock_Config(void);
static void Crypto_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef* hdma);
static void DMA_CH1_Init(DMA_InitTypeDef* hdma);
static void DMA_Init(void);
static void USART_Init();

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
    /* Зашифровать данные */
    HAL_DMA_Start(&hdma_ch0, plain_text, (void*)&hcrypto.Instance->BLOCK, sizeof(plain_text) - 1);
    HAL_DMA_Start(&hdma_ch1, (void*)&hcrypto.Instance->BLOCK, cipher_text, sizeof(cipher_text) - 1);


    if (HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT) != HAL_OK)
    {
        xprintf("Timeout CH0, error %d\n", HAL_DMA_GetBusError(&hdma_ch0));
        return;
    }

    if (HAL_DMA_Wait(&hdma_ch1, DMA_TIMEOUT_DEFAULT) != HAL_OK)
    {
        xprintf("Timeout CH1\n");
        return;
    }
    

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

    USART_Init();

    Crypto_Init();
    
    DMA_Init();

    HAL_Crypto_CounterReset(&hcrypto);
    dma_kuznechik_ECB_code();
    
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
    PCC_OscInit.LSI32KCalibrationValue = 128;
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


static void DMA_CH0_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch0.dma = hdma;

    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;  
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;  

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD;       /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                     /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_CRYPTO_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD;     /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                    /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_CRYPTO_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;  

}

static void DMA_CH1_Init(DMA_InitTypeDef* hdma)
{
    hdma_ch1.dma = hdma;

    /* Настройки канала */
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

}


static void DMA_Init(void)
{
    
    /* Настройки DMA */
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_DISABLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        xprintf("DMA_Init Error\n");
    }


    /* Инициализация канала */
    DMA_CH0_Init(&hdma);
    DMA_CH1_Init(&hdma);

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
