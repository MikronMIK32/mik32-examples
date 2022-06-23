#include "common.h"
#include "i2c.h"
#include "stdbool.h"


void i2c_slave_init(I2C_TypeDef* i2c, uint8_t slave_adres)
{
    slave_adres = slave_adres << 1;

    // Включаем тактирование необходимых блоков
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_I2C_0_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M ;
    
    // обнуление регистра управления
    i2c->CR1 = 0;

    /*
    *
    * I2C_OAR1 - регистр собственного адреса
    * 
    * OA1 - собственный адрес 1
    * 
    * OA1MODE - режим 7/10 бит адреса OA1. 0 - 7 бит
    * 
    * OA1EN - использование собствевнного адреса OA1. 1 - при получении адреса OA1 - ACK 
    * 
    */
    i2c->OAR1 = (slave_adres << I2C_OAR1_OA1_S) | (1 << I2C_OAR1_OA1EN_S); // собственный адрес 
    

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
    i2c->CR1 |= I2C_CR1_PE_M;

    xprintf("\nВедомый. Старт\n");

}

void i2c_slave_restart(I2C_TypeDef* i2c)
{
    // Получение адреса ведомого
    uint8_t slave_adres = (uint8_t)(i2c->OAR1 & (0b1111111111));
    slave_adres >>= 1;
    xprintf("Рестарт. adres = 0x%02x\n", slave_adres);
    
    // Программный сброс модуля i2c
    i2c->CR1 &= ~I2C_CR1_PE_M;
    for (volatile int i = 0; i < 1000000; i++); 

    // Повторная инициализация
    i2c_slave_init(i2c, slave_adres);
}

void i2c_slave_write(I2C_TypeDef* i2c, uint8_t data[], uint8_t byte_count)
{

    xprintf("\nОтправка %d\n", data[0]<<8 | data[1]);

    for (uint8_t i = 0; i < byte_count; i++)
    {
        int counter = 0; // Счетчик для ожидания
        while(!(i2c->ISR & I2C_ISR_TXIS_M)) // TXIS = 1 - регистр TXDR свободен
        {
            counter++;
            if(counter == 1000000)
            {
                // Ожидание превышено. Возможно механическое повреждение линии связи
                break;
            }

            // При записи байта мастер прислал NACK. Возникла ошибка при передаче
            if(i2c->ISR & I2C_ISR_NACKF_M)
            {
                xprintf("Запись. NACKF = %d\n", (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
                break;
            }
        }

        if(counter == 1000000)
        {
            xprintf("Разрыв связи\n");

            // Программный сброс модуля i2c и его повторная инициалиизация 
            i2c_slave_restart(i2c);
            break;
        } 

        // При записи байта мастер прислал NACK. Возникла ошибка при передаче
        if(i2c->ISR & I2C_ISR_NACKF_M)
        {
            i2c->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF и NACKF
            xprintf("Ошибка при передаче\n");
            break;
        }

        i2c->TXDR = data[i]; // Загрузка передаваемого байта в регистр TXDR

        xprintf("Отправлен байт 0x%02x\n", data[i]);
        i2c->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF
    }


}

void i2c_slave_read(I2C_TypeDef* i2c, uint8_t data[], uint8_t byte_count)
{

    xprintf("\nЧтение\n");

    for (uint8_t i = 0; i < byte_count; i++)
    {
        // Счетчик для ожидания
        int counter = 0;

        // Байт нужно прочитать когда RXNE = 1
        while(!(i2c->ISR & I2C_ISR_RXNE_M))
        {
            
            counter++;
            if(counter == 1000000)
            {
                // Ожидание превышено. Возможно механическое повреждение линии связи
                break;
            }

            if((i2c->ISR & I2C_ISR_NACKF_M))
            {
                // Мастер прислал NACK. Ошибка чтения
                break;
            }

        } 

        // Ожидание превышено. Возможно механическое повреждение линии связи
        if(counter == 1000000)
        {
            xprintf("Разрыв связи\n");

            // Программный сброс модуля i2c и его повторная инициалиизация 
            i2c_slave_restart(i2c);
            break;
        }

        if((i2c->ISR & I2C_ISR_NACKF_M))
        {
            xprintf("Ошибка чтения\n");
            break;
        }

        data[i] = i2c->RXDR; // Чтение байта и сброс флага RXNE
        xprintf("Чтение байта  0x%02x\n", data[i]);
    }

    xprintf("Прочитано  %d\n", data[0] << 8 | data[1]);
}

int main()
{

    // Адрес ведомого
    uint8_t slave_address = 0x36;
    // Число для оптавки
    uint16_t to_send = 0; 
    // Массив с байтами для отправки / приема
    uint8_t data[2];

    // инициализация блока i2c в режиме слейв (ведомый)
    i2c_slave_init(I2C_0, slave_address); 
    
    while (1)
    {
        xprintf("\nОжидание\n");
        int counter = 0;

        // Ожидание запроса мастером адреса слейва
        while(!(I2C_0->ISR & I2C_ISR_ADDR_M))
        {
            counter++;
            if(counter==10000000)
            {
                xprintf("\nОжидание\n");
                counter = 0;
            }
            
        } 
        
        //I2C_0 -> ISR |=  I2C_ISR_TXE_M;

        // срос флага ADDR для подверждения принятия адреса
        I2C_0->ICR |= I2C_ICR_ADDRCF_M; 

        /*
        * I2C_ISR - Регистр прерываний и статуса
        *   
        * ADDR - Флаг соответствия адреса в ре-жиме ведомого
        *   
        * DIR - Тип передачи. Обновляется в режиме ведомого при получении адреса:
        *       0 – передача типа запись, ведомый переходит в режим приемника;
        *       1 – передача типа чтения, ведомый переходит в режим передатчика
        * 
        */

        if(I2C_0->ISR & I2C_ISR_DIR_M) // DIR = 1. Режим ведомого - передатчик
        {
            xprintf("\nЗапрос на запись\n");

            // Отправляется число to_send, состоящее из двух байт
            // Заполняется массив data байтами, которые нужно отправить
            data[0] = to_send >> 8;
            data[1] = to_send & 0b0000000011111111;

            // Отправка
            i2c_slave_write(I2C_0, data, sizeof(data));
        } 
        else // DIR = 0. Режим ведомого - приемник
        {
            xprintf("\nЗапрос на чтение\n");
            
            // Чтение двух байт от мастера и запись их в массив data 
            i2c_slave_read(I2C_0, data, sizeof(data));

            // Формирование принятого числа
            to_send = data[0] << 8 | data[1];
            to_send++; 
            if(to_send > 65530)
            {
                to_send = 0;
            }
        }

    }
    
    
}