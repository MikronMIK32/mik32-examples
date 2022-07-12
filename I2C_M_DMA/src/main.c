#include "common.h"
#include "i2c.h"
#include "stdbool.h"
#include "dma_config.h"


#define no_shift false
#define DMA_CHANNEL_0     0
#define DMA_CHANNEL_1     1
#define DMA_CHANNEL_2     2
#define DMA_CHANNEL_3     3


typedef enum
{
    i2c_dma_tx,
    i2c_dma_rx,
} i2c_dma; 


void i2c_master_init(I2C_TypeDef* i2c)
{
    //Включаем тактирование необходимых блоков и модуля выбора режима GPIO 
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_I2C_0_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M ;
    for (volatile int i = 0; i < 10; i++);


    // обнуление регистра управления
    i2c->CR1 = 0;

    /*
    * Инициализация i2c
    * TIMING - регистр таймингов
    * 
    * SCLDEL - Задержка между изменением SDA и фронтом SCL в режиме ведущего и ведомого при NOSTRETCH = 0
    * 
    * SDADEL - Задержка между спадом SCL и изменением SDA в режиме ведущего и ведомого при NOSTRETCH = 0
    * 
    * SCLL - Время удержания SCL в состоянии логического «0» в режиме веедущего
    * 
    * SCLH - Время удержания SCL в состоянии логической «1» в режиме веедущего
    * 
    * PRESC - Делитель частоты I2CCLK. Используется для вычесления периода сигнала TPRESC для счетчиков предустановки, 
    * удержания, уровня «0»и «1»
    * 
    */
    i2c->TIMINGR = I2C_TIMINGR_SCLDEL(1) | I2C_TIMINGR_SDADEL(1) |
        I2C_TIMINGR_SCLL(20) | I2C_TIMINGR_SCLH(20) | I2C_TIMINGR_PRESC(3); //частота 164,7 кГц tsync1 + tsync2 = 10^(-6)

    /*
    *
    * CR1 - Регистр управления
    * 
    * PE - Управление интерфейсом: 0 – интерфейс выключен; 1 – интерфейс включен
    *
    */
    i2c->CR1 = I2C_CR1_PE_M;
    xprintf("\nМастер. Старт\n");
}
void i2c_master_restart(I2C_TypeDef* i2c)
{
    // Программный сброс модуля i2c
    i2c->CR1 &= ~I2C_CR1_PE_M;
    for (volatile int i = 0; i < 1000000; i++); 

    // Повторная инициализация
    i2c_master_init(i2c);
}
void i2c_master_DMA_mode(I2C_TypeDef* i2c, i2c_dma dma_mode)
{
    switch (dma_mode)
    {
        case i2c_dma_tx:
        I2C_0->CR1 |= I2C_CR1_TXDMAEN_M;
        break;
        case i2c_dma_rx:
        I2C_0->CR1 |= I2C_CR1_RXDMAEN_M;
        break;
        default:
        xprintf("Неизвестный режим поддержки DMA");
    }
}

void DMA_Channels_init(
    DMA_CONFIG_TypeDef* dma, 
    void* tx_address, void* rx_address, 
    int dma_request_index, uint8_t DMA_Channel, uint8_t data[],uint32_t count,
    uint32_t common_config, i2c_dma i2c_dma_mode
)
{
    // ConfigStatus - регистр DMA_CNTR
    dma->ConfigStatus = 0xFFFFFFFF;

    /*
    *
    * common_config - регистр CHx_CFG
    * 
    * DMA_CFG_CH_ENABLE_M - Разрешение работы канала:
    * 
    * DMA_CFG_CH_WRITE_REQ - Write requets. Выбор периферийной линии назначения. 5 для I2C_0
    * 
    * DMA_CFG_CH_READ_REQ - Read requets. Выбор периферийной линии источника.
    * 
    */
    common_config |= DMA_CFG_CH_ENABLE_M | 
        DMA_CFG_CH_WRITE_REQ(dma_request_index) |
        DMA_CFG_CH_READ_REQ(dma_request_index);
    /*
    * Выбор канала DMA
    * Создаем указатель типа DMA_CHANNEL_TypeDef и присваиваем ему 
    * адрес нулевого канала dma с наивысшим приоритетом 
    * 
    */

    DMA_CHANNEL_TypeDef* dma_ch;
    switch (DMA_Channel)
    {
    case DMA_CHANNEL_0:
        dma_ch = &(dma->CHANNELS[DMA_CHANNEL_0]);
        break;
    case DMA_CHANNEL_1:
        dma_ch = &(dma->CHANNELS[DMA_CHANNEL_1]);
        break;
    case DMA_CHANNEL_2:
        dma_ch = &(dma->CHANNELS[DMA_CHANNEL_2]);
        break;
    case DMA_CHANNEL_3:
        dma_ch = &(dma->CHANNELS[DMA_CHANNEL_3]);
        break;
    }
    

    /*
    *
    * DESTINATIONS - CHx_DST. Регистр адреса назначения канала
    * 
    * SOURCE - CHx_SRC. Регистр адреса источника канала
    * 
    * LEN - CHx_LEN. Регистр размера передаваемых данных канала. 
    * Количество байт пересылки расчитывается как Data_Len +1
    * *******************
    * CONFIG - CHx_CFG. Регистр управления и конфигурации канала
    * 
    * DMA_CFG_CH_READ_MODE_memory_M - Read mode. Режим адреса источника:
    * 
    * DMA_CFG_CH_READ_INCREMENT_M - Read incr. Инкремент адреса источника:
    * 
    * DMA_CFG_CH_WRITE_MODE_periphery_M - Write mode. Режим адреса назначения:
    * 
    * DMA_CFG_CH_WRITE_no_INCREMENT_M - Write incr. Инкремент адреса назначения:
    * 
    */

    switch(i2c_dma_mode)
    {
        case i2c_dma_tx:
            dma_ch->SOURCE = (uint32_t)data;
            dma_ch->DESTINATIONS = (uint32_t)(tx_address);
            dma_ch->LEN = count-1;
            dma_ch->CONFIG = common_config | 
            DMA_CFG_CH_READ_MODE_memory_M | DMA_CFG_CH_READ_INCREMENT_M |
            DMA_CFG_CH_WRITE_MODE_periphery_M | DMA_CFG_CH_WRITE_no_INCREMENT_M;
        break;
        case i2c_dma_rx:
            dma_ch->SOURCE = (uint32_t)(rx_address);
            dma_ch->DESTINATIONS = (uint32_t)data;
            dma_ch->LEN = count-1;
            dma_ch->CONFIG = common_config | 
                DMA_CFG_CH_READ_MODE_periphery_M | DMA_CFG_CH_READ_no_INCREMENT_M |
                DMA_CFG_CH_WRITE_MODE_memory_M | DMA_CFG_CH_WRITE_INCREMENT_M;
        break;
    }

}

void DMA_Master_Wait(DMA_CONFIG_TypeDef* dma, i2c_dma i2c_dma_mode)
{
    uint32_t timeout = 10000000; 

    /*
    *
    * Последние 4 бита ConfigStatus - Статус состояния каналов:
    * «1» - готов к работе;
    * «0» - занят
    * 
    */
    switch(i2c_dma_mode)
    {
        case i2c_dma_tx:
            while (((dma->ConfigStatus & DMA_STATUS_READY(DMA_CHANNEL_0)) == 0) && (--timeout < 0));
        break;
        case i2c_dma_rx:
            while (((dma->ConfigStatus & DMA_STATUS_READY(DMA_CHANNEL_0)) == 0) && (--timeout < 0)); 
        break;
    }

    if (timeout < 0)
    {
        xprintf("DMA: ожидание готовности канала превышено\n");
    }
}

void DMA_check_data(uint8_t data[], int count, i2c_dma i2c_dma_mode)
{
    for(int i = 0; i < count; i++)
    {
        switch(i2c_dma_mode)
        {
            case i2c_dma_tx:
            xprintf("Отправка байта[%d]  0x%02x\n", i, data[i]);
            break;
            case i2c_dma_rx:
            xprintf("Чтение байта[%d]  0x%02x\n", i, data[i]);
            break;
        }
    }

    switch(i2c_dma_mode)
    {
        case i2c_dma_tx:
        xprintf("Отправлено %d\n\n", data[0] << 8 | data[1]);
        break;
        case i2c_dma_rx:
        xprintf("Получено %d\n\n", data[0] << 8 | data[1]);
        break;
    }
}

void i2c_master_DMA(I2C_TypeDef* i2c, int dma_request_index, uint8_t data[], 
                    uint32_t count, uint8_t slave_address, i2c_dma i2c_dma_mode, 
                    uint8_t DMA_Channel, bool shift)
{
    uint8_t slave_adr = slave_address;
    // shift - true когда адрес ведомого должен быть сдвинут на 1 бит
    if(!shift)
    {
        slave_adr = slave_adr << 1;
    }
    

    DMA_CONFIG_TypeDef* dma = DMA_CONFIG;
    /*
    * DMA_CFG_CH_READ_SIZE_byte_M - Read size - Разрядность адреса источника:
    * 
    * DMA_CFG_CH_READ_BURST_SIZE_S - Read block size
    * Количество байт в пакете. Определяется как 2^Read block size
    * 
    * DMA_CFG_CH_WRITE_SIZE_byte_M - Write size. Разрядность адреса назначения
    */
    uint32_t config =   DMA_CFG_CH_READ_SIZE_byte_M |(0 << DMA_CFG_CH_READ_BURST_SIZE_S) | 
                        DMA_CFG_CH_WRITE_SIZE_byte_M |(0 << DMA_CFG_CH_WRITE_BURST_SIZE_S);

    DMA_Channels_init(dma, (void*)&i2c->TXDR, (void*)&i2c->RXDR, dma_request_index, DMA_Channel, data, count, config, i2c_dma_mode);

    switch (i2c_dma_mode)
    {
    case i2c_dma_tx:
        i2c->CR2 = I2C_CR2_SADD(slave_adr) | I2C_CR2_WR_M | I2C_CR2_NBYTES(count) | I2C_CR2_AUTOEND_M;
        break;
    case i2c_dma_rx:
        i2c->CR2 = I2C_CR2_SADD(slave_adr) | I2C_CR2_RD_M | I2C_CR2_NBYTES(count) | I2C_CR2_AUTOEND_M;
        break;
    }

    i2c->CR2 |= I2C_CR2_START_M; // старт отправки адреса, а затем данных 
    
    DMA_Master_Wait(dma, i2c_dma_mode);
    DMA_check_data(data, count, i2c_dma_mode);
}

int main ()
{
    // Адрес ведомого
    uint8_t slave_address = 0x36; 

    // Число для оптавки
    uint16_t to_send = 65000; 
    
    // Массив с байтами для отправки / приема
    uint8_t data[2];
    data[0] = to_send >> 8; // массив заполняется байтами числа to_send
    data[1] = to_send & 0b0000000011111111; // массив заполняется байтами числа to_send

    // Инициализация блока i2c в режиме мастер (ведущий)
    i2c_master_init(I2C_0);

    i2c_dma i2c_dma_mode;
    
    

    while (1)
    {
        i2c_dma_mode = i2c_dma_tx;
        i2c_master_DMA_mode(I2C_0, i2c_dma_mode);
        i2c_master_DMA(I2C_0, DMA_I2C_0_INDEX, data, sizeof(data), slave_address, i2c_dma_mode, DMA_CHANNEL_0, no_shift); 
        for (volatile int i = 0; i < 1000000; i++); 

        i2c_dma_mode = i2c_dma_rx;
        i2c_master_DMA_mode(I2C_0, i2c_dma_mode);
        i2c_master_DMA(I2C_0, DMA_I2C_0_INDEX, data, sizeof(data), slave_address, i2c_dma_mode, DMA_CHANNEL_0, no_shift); 
        for (volatile int i = 0; i < 1000000; i++);

    }
    
}