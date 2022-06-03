#include "common.h"
#include "i2c_loop_common.h"
#include "stdbool.h"
#include "wakeup.h"

void i2c_init_slave(I2C_TypeDef* i2c, uint8_t slave_adres)
{
    //Включаем тактирование необходимых блоков - GPIO_0, GPIO_1, GPIO_2 и модуля выбора режима GPIO 
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_0_M | PM_CLOCK_I2C_0_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M | PM_CLOCK_WU_M | PM_CLOCK_PM_M ;
    
    // обнуление регистра управления
    i2c->CR1 = 0;

    // почему то тут нет #define I2C_OAR1_OA1(v)             (((v) << I2C_OAR1_OA1_S) & I2C_OAR1_OA1_M)

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
    * STOPIE - Разрешение прерывания детектировании STOP: 0 – прерывание запрещено; 1 – прерывание разрешено
    *
    */
    //i2c->CR1 = I2C_CR1_NOSTRETCH_M;
    i2c->CR1 |= I2C_CR1_PE_M;

    xprintf("\nВедомый. Старт\n");

}

void i2c_slave_write(I2C_TypeDef* i2c, uint8_t data)
{

    xprintf("Отправка %d\n", data);
    //xprintf("До цикла TXIS = %d, NACKF = %d, STOPF = %d\n", (i2c->ISR & I2C_ISR_TXIS_M)>>I2C_ISR_TXIS_S, (i2c->ISR & I2C_ISR_NACKF_M)>>I2C_ISR_NACKF_S ,(i2c->ISR & I2C_ISR_STOPF_M)>>I2C_ISR_STOPF_S);
    while(!(i2c->ISR & I2C_ISR_TXIS_M)) // TXIS = 1 - регистр TXDR свободен
    {
            //xprintf("В цикле TXIS = %d, NACKF = %d, STOPF = %d\n", (i2c->ISR & I2C_ISR_TXIS_M)>>I2C_ISR_TXIS_S, (i2c->ISR & I2C_ISR_NACKF_M)>>I2C_ISR_NACKF_S ,(i2c->ISR & I2C_ISR_STOPF_M)>>I2C_ISR_STOPF_S);
            if(i2c->ISR & I2C_ISR_NACKF_M)//при записи байта мастер прислал stop
            {
                xprintf("Запись. NACKF = %d\n", (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
                break;
            }
    } 

    if(i2c->ISR & I2C_ISR_NACKF_M)//при записи байта мастер прислал STOP
    {
        i2c->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF и NACKF
        xprintf("Сброс STOPF и NACKF\n");
        //break;
        return;
    }

    //xprintf("Загрузка %d в TXDR\n", data);
    i2c->TXDR = data; // первая загрузка в буфер
    xprintf("Отправлено %d\n", data);
    i2c->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF

}

uint8_t i2c_slave_read(I2C_TypeDef* i2c)
{
    uint8_t data = 0;
    uint8_t counter = 0;

    //байт нужно прочитать когда RXNE = 1
    while(!(i2c->ISR & I2C_ISR_RXNE_M))
    {
        //xprintf("RXNE = %d\n", (i2c->ISR & I2C_ISR_RXNE_M)>>I2C_ISR_RXNE_S);

        // counter++;
        // if(counter>100)
        // break;

        if((i2c->ISR & I2C_ISR_STOPF_M))
        {
            xprintf("Чтение. STOPF = %d, NACKF = %d\n", (i2c->ISR & I2C_ISR_STOPF_M) >> I2C_ISR_STOPF_S, (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
            break;
        }

    } 
        if((i2c->ISR & I2C_ISR_NACKF_M))
        {
            xprintf("Ошибка чтения\n");
            return data;
        }
        // if(i2c->ISR & I2C_ISR_STOPF_M)
        // {
        //     i2c->ICR |= I2C_ICR_STOPCF_M | I2C_ICR_NACKCF_M; // сброс флага STOPF и сброс флага NACKF
        //     xprintf("Сброс. STOPF = %d, NACKF = %d\n", (i2c->ISR & I2C_ISR_STOPF_M) >> I2C_ISR_STOPF_S, (i2c->ISR & I2C_ISR_NACKF_M) >> I2C_ISR_NACKF_S);
        //     //break;
        //     //return;
        // }
    //xprintf("RXNE = %d\n", (i2c->ISR & I2C_ISR_RXNE_M)>>I2C_ISR_RXNE_S);
    // for(volatile int i = 0; i < 100; i++)
    // {
    //     if (i2c->ISR & I2C_ISR_RXNE_M)
    //     {
    //         break;
    //     }
    // }

    data = i2c->RXDR; // чтение байта и сброс RXNE
    xprintf("Чтение %d\n", data);
    return data;
}

int main()
{
    // PM->AHB_CLK_MUX = 0; // выбор источника тактирования. 0 - внешний кварц
    // PM->CPU_RTC_CLK_MUX = 0;// выбор часового кварца как внешний
    // for (volatile int i = 0; i < 100; i++); 
    // WU->CLOCKS_SYS = 1 << 1; // выключение внутреннего системного кварца
    // WU->CLOCKS_BU = 1 << 1; // выключение внутреннего часовго кварца
    // for (volatile int i = 0; i < 100; i++) ; 
    
    
    uint8_t slave_adres = 0x36;
    uint8_t data_byte = 3;

    i2c_init_slave(I2C_0, slave_adres<<1); // инициализация блока i2c в режиме slave 
    
    while (1)
    {
        int counter = 0;
        xprintf("\nЖду\n");
        // мастер запросил адрес слейва
        while(!(I2C_0->ISR & I2C_ISR_ADDR_M))
        {
            counter++;
            if(counter==10000000)
            {
                xprintf("\nЖду\n");
                counter = 0;
            }
            
        } 
        
        if((I2C_0->ISR & I2C_ISR_ADDR_M) == I2C_ISR_ADDR_M)
        {
            xprintf("ADDR = 1\n");
            //xprintf("До сброса ADDR. TXIS = %d, NACKF = %d, STOPF = %d\n", (I2C_0->ISR & I2C_ISR_TXIS_M)>>I2C_ISR_TXIS_S, (I2C_0->ISR & I2C_ISR_NACKF_M)>>I2C_ISR_NACKF_S ,(I2C_0->ISR & I2C_ISR_STOPF_M)>>I2C_ISR_STOPF_S);
        }
        I2C_0 -> ISR |=  I2C_ISR_TXE_M;
        I2C_0->ICR |= I2C_ICR_ADDRCF_M; // срос ADDR
        //xprintf("Сброс. ADDR = %d\n", (I2C_0->ISR & I2C_ISR_ADDR_M) >> I2C_ISR_ADDR_S);
        //xprintf("После сброса ADDR. TXIS = %d, NACKF = %d, STOPF = %d\n", (I2C_0->ISR & I2C_ISR_TXIS_M)>>I2C_ISR_TXIS_S, (I2C_0->ISR & I2C_ISR_NACKF_M)>>I2C_ISR_NACKF_S ,(I2C_0->ISR & I2C_ISR_STOPF_M)>>I2C_ISR_STOPF_S);
        
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
            i2c_slave_write(I2C_0, data_byte);
        } 
        else // DIR = 0. Режим ведомого - приемник
        {
            xprintf("\nЗапрос на чтение\n");
            data_byte = i2c_slave_read(I2C_0);
            data_byte++; 
            if(data_byte > 250)
            {
                data_byte = 0;
            }
        }

    }
    
    
}