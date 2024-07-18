#include "mik32_hal.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_spifi_w25.h"
#include "riscv_csr_encoding.h"
#include "csr.h"

#include "uart_lib.h"
#include "xprintf.h"

/* 
 *        !!! Полный текст описания читайте ниже кода программы !!!
 *
 * Пример начального загрузчика для использования SPIFI в связке с внешней МФП
 * (Микросхемой Флеш-Памяти) в роли памяти программ, адресуемой автоматически.
 * В примере запускается тактирование SPIFI и настраиваются выводы, включается
 * кэширование и устанавливается адрес входа в прерывание. После этого следует
 * инструкция выборки команд из начального адресного пространства внешней МФП.
 *  
 *  Варианты настройки константы "CHIP_MODE":
 * 
 *  0 = Автовыбор по итогу проверки связи: [SPI]->[QDO]->[QIO]->[QPI]->[XIP]
 *  1 = SPI — Весь обмен по одной линии связи. Это самый медленный вариант.
 *  2 = QDO — По четырём линиям связи передаются только данные.
 *  3 = QIO — По четырём линиям не передаются только команды.
 *  4 = QPI — Весь обмен осуществляется в четырёхпроводном режиме.
 *  5 = XIP — Весь обмен четырёхпроводной, но вообще без передачи команд.
 * 
 *  Любое значение вне диапазона чисел от 0 до 5 будет восприниматься как 0.
 * 
 */

#define CHIP_MODE 0            /* Выбор протоколов связи c МФП (варианты: 0 ~ 5) */
#define CHIP_SIZE 8*1024*1024  /* Объём внешней флеш-памяти (от 128 кБ до 16 МБ) */
#define TEST_ADDR 0            /* Начальный адрес проверки (по всему объёму МФП) */
#define TEST_SIZE 256          /* Размер тестируемого участка (от 1 до 256 байт) */

#define XPRINTF_UART UART_1
#define XPRINTF_UART_SPEED 9600
#define XPRINTF_UART_DEBUG 0

void SystemClock_Config();
void SPIFI_Init();

int main() {

    HAL_Init();

    SystemClock_Config();

    UART_Init(XPRINTF_UART, (32000000/XPRINTF_UART_SPEED), UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0, XPRINTF_UART_DEBUG);

    xprintf_uarts_allow(XPRINTF_UART_1);

    xprintf("\nEEPROM->SPIFI Bootloader Started with ");

    SPIFI_Init();

    write_csr(mtvec, 0x80000000);

    asm volatile("la ra, 0x80000000\n\t"
                 "jalr ra");
    
    xprintf("Error 3\n\n");
    while (1);
}

void SystemClock_Config(void) {
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

#if CHIP_MODE < 0
#define CHIP_MODE 0
#elif CHIP_MODE > 5
#define CHIP_MODE 0
#endif

#if CHIP_SIZE > 16 * 1024 * 1024
#define CHIP_SIZE 16 * 1024 * 1024
#elif CHIP_SIZE < 128 * 1024
#define CHIP_SIZE 128 * 1024
#endif

#if TEST_SIZE > 256
#define TEST_SIZE 256
#elif TEST_SIZE < 1
#define TEST_SIZE 1
#endif

#if TEST_ADDR > CHIP_SIZE - TEST_SIZE
#define TEST_ADDR CHIP_SIZE - TEST_SIZE
#elif TEST_ADDR < 0
#define TEST_ADDR 0
#endif

#define AUTO_MODE 0
#define SPI_MODE 1
#define QDO_MODE 2
#define QIO_MODE 3
#define QPI_MODE 4
#define XIP_MODE 5

uint8_t spi_bytes[TEST_SIZE] = {};
uint8_t tmp_bytes[TEST_SIZE] = {};
uint8_t chip_mode = 0;

bool cmp_bytes(uint8_t *bytes_a, uint8_t *bytes_b, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        if (bytes_a[i] != bytes_b[i]) {
            return 0;
        }
    }
    return 1;
}

void clean_bytes(uint8_t *bytes_x) {
    for (uint16_t i = 0; i < TEST_SIZE; i++) {
        bytes_x[i] = 0;
    }
}

void SPIFI_Init() {
    SPIFI_HandleTypeDef spifi = {.Instance = SPIFI_CONFIG};
    HAL_SPIFI_MspInit(&spifi);
    HAL_SPIFI_Reset(&spifi);

    HAL_SPIFI_W25_Reset_QPI(&spifi);

    clean_bytes(spi_bytes);
    HAL_SPIFI_W25_ReadData(&spifi, TEST_ADDR, TEST_SIZE, spi_bytes);
    uint16_t count_00 = TEST_SIZE;
    uint16_t count_ff = TEST_SIZE;
    for (uint16_t i = 0; i < TEST_SIZE; i++) {
        if (spi_bytes[i] == 0x00) {
            count_00--;
        }
        if (spi_bytes[i] == 0xFF) {
            count_ff--;
        }
    }
    
    if (count_00 < 1 || count_ff < 1) {
        xprintf("Error 0\n\n");
        while (1);
    }

    clean_bytes(tmp_bytes);
    HAL_SPIFI_W25_ReadData(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
    if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 0) {
        xprintf("Error 1\n\n");
        while (1); 
    }
    chip_mode = SPI_MODE;
    SPIFI_MemoryCommandTypeDef cmd_mem = {
        .FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_ALL_SERIAL,
        .FrameForm = SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR,
        .InterimData = 0
    };
#if CHIP_MODE != SPI_MODE
    if (HAL_SPIFI_W25_QuadEnable(&spifi) == HAL_OK) {
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == QDO_MODE
        clean_bytes(tmp_bytes);
        HAL_SPIFI_W25_ReadData_Quad(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
        if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
            chip_mode = QDO_MODE;
        }
#endif
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == QIO_MODE
        clean_bytes(tmp_bytes);
        HAL_SPIFI_W25_ReadData_Quad_IO(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
        if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
            chip_mode = QIO_MODE;
        }
#endif
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == QPI_MODE
        clean_bytes(tmp_bytes);
        HAL_SPIFI_W25_QPIEnable(&spifi);
        HAL_SPIFI_W25_ReadData_Quad_IO_QPI(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
        if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
            chip_mode = QPI_MODE;
        } else {
            HAL_SPIFI_W25_QPIDisable(&spifi);
        }
#endif
#if CHIP_MODE == AUTO_MODE
        if (chip_mode == QPI_MODE) {
            clean_bytes(tmp_bytes);
            HAL_SPIFI_W25_ReadData_Quad_IO_QPI_XIP_Init(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
            if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
                clean_bytes(tmp_bytes);
                HAL_SPIFI_W25_ReadData_Quad_IO_QPI_XIP(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
                if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
                    chip_mode = XIP_MODE;
                }
            }
        }
#endif
#if CHIP_MODE == XIP_MODE
        clean_bytes(tmp_bytes);
        HAL_SPIFI_W25_QPIEnable(&spifi);
        HAL_SPIFI_W25_ReadData_Quad_IO_QPI_XIP_Init(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
        if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
            clean_bytes(tmp_bytes);
            HAL_SPIFI_W25_ReadData_Quad_IO_QPI_XIP(&spifi, TEST_ADDR, TEST_SIZE, tmp_bytes);
            if (cmp_bytes(tmp_bytes, spi_bytes, TEST_SIZE) == 1) {
                chip_mode = XIP_MODE;
            } else {
                HAL_SPIFI_W25_Reset_QPI(&spifi);
            }
        } else {
            HAL_SPIFI_W25_Reset_QPI(&spifi);
        }
#endif
    }
#endif
    switch (chip_mode) {
    case (SPI_MODE):
        cmd_mem.InterimLength = 0;
        cmd_mem.OpCode = 0x03;
        xprintf("SPI Mode\n\n");
        break;
#if CHIP_MODE != SPI_MODE
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == QDO_MODE
    case (QDO_MODE):
        cmd_mem.FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_DATA_PARALLEL;
        cmd_mem.InterimLength = 1;
        cmd_mem.OpCode = 0x6B;
        xprintf("QDO Mode\n\n");
        break;
#endif
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == QIO_MODE
    case (QIO_MODE):
        cmd_mem.FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_OPCODE_SERIAL;
        cmd_mem.InterimLength = 3;
        cmd_mem.OpCode = 0xEB;
        xprintf("QIO Mode\n\n");
        break;
#endif
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == QPI_MODE
    case (QPI_MODE):
        cmd_mem.FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_ALL_PARALLEL;
        cmd_mem.InterimLength = 1;
        cmd_mem.OpCode = 0xEB;
        xprintf("QPI Mode\n\n");
        break;
#endif
#if CHIP_MODE == AUTO_MODE | CHIP_MODE == XIP_MODE
    case (XIP_MODE):
        cmd_mem.FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_ALL_PARALLEL;
        cmd_mem.FrameForm = SPIFI_CONFIG_CMD_FRAMEFORM_NOOPCODE_3ADDR;
        cmd_mem.InterimLength = 1;
        cmd_mem.InterimData = 0x20;
        cmd_mem.OpCode = 0xEB;
        xprintf("XIP Mode\n\n");
        break;
#endif
#endif
    default:
        xprintf("Error 2\n\n");
        while (1);
    }

    SPIFI_MemoryModeConfig_HandleTypeDef spifi_mem = {
        .Instance = spifi.Instance,
        .CacheEnable = SPIFI_CACHE_ENABLE,
        .CacheLimit = 0x90000000,
        .Command = cmd_mem
    };

    HAL_SPIFI_MemoryMode_Init(&spifi_mem);
}

/*
 * Данный пример демонстрирует настройку интерфейса SPIFI в связке с внешней
 * микросхемой флеш-памяти (далее МФП — сокращение от Микросхема Флеш-Памяти).
 * 
 * Пример начального загрузчика для использования SPIFI в связке с внешней МФП
 * в роли памяти программ и данных, адресуемой микроконтроллером автоматически.
 * В примере запускается тактирование SPIFI и настраиваются выводы, включается
 * кэширование и устанавливается адрес входа в прерывание. После чего выполняется
 * команда перехода программы в начало адресного пространства внешней флеш-памяти.
 * Реализована проверка участка адресного пространства внешней МФП на предмет
 * наличия разнообразных данных. Если все биты полученных байтов состоят только
 * из нулей или только из единиц, значит или микросхема была так записана, или
 * она неисправна, или же не подключена. Тестируемый участок памяти считывается
 * дважды с целью сверки полного соответствия первого варианта со вторым. Это
 * необходимо, чтобы не принимать случайные шумовые сигналы на линиях связи у
 * SPIFI за реальные данные из МФП. Неслучайность доказана, если оба варианта
 * совпали. Следующим шагом проверяем бит разрешения работы МФП в четырёхпро-
 * водном режиме (QUAD ENABLE). Если бит уже установлен или получилось его
 * установить, то считывается тот же участок памяти, но уже в режиме "Quad Data
 * Output", т.е. передачи команд и адресов идут по одной линии, а возвращаемые
 * от МФП данные идут по четырём линиям связи. Считанные данные всегда сверяются
 * с ранее полученными данными в режиме SPI. Если данные в порядке, значит режим
 * "Quad Data Output" работоспособен. Далее также тестируется и более быстрый
 * режим "Quad Input/Output", где кроме команды всё остальное передаётся по
 * четырём линиям связи. Самый быстродействующий потокол связи это "QPI", в
 * котором весь обмен происходит только в четырёхпроводном режиме. В итоге своей
 * работы программа выберет тот режим связи с внешней МФП, который работает
 * быстрее и без ошибок. Хотя в настройках вы можете выбрать любой нужный режим,
 * который запустится, если его тест будет пройден успешно.
 * 
 * Программа зацикливается при обнаружении ошибок, нежелательных для продолжения
 * дальнейшей работы. Это может помочь избежать повреждений портов ввода-вывода,
 * через которые внешняя МФП подключена к SPIFI модулю в составе чипа MIK32.
 * 
 * Пример протестирован на отладочной плате "NUKE MIK32 V0.3" со встроенным
 * программатором на базе FT2232HL. В составе платы МФП типа W25Q64FV на 8 МБ.
 * W25Q64FV прошивалась без проблем даже на максимальной скорости связи между
 * программатором и микроконтроллером, а это 4 МГц. Судя по документации безо
 * всяких проблем и изменений кода данной программы подойдут аналоги таких
 * серий как: W25Q16JV-DTR, W25Q32FV, W25Q64FV, W25Q128FV, W25Q256FV, а также
 * W25Q512JV-DTR. Все они поддерживают режим QPI, т.е. максимум быстродействия
 * интерфейса SPIFI для MIK32 Амур. Небольшие изменения данного примера дадут
 * возможность использовать адресное пространство больше 16 МБ, режим адресации
 * которого всего 24 бита. Переключение в режим 32 бита расширяет программную
 * память до 2 ГБ из МФП с интерфейсами типа DUAL/QUAD SPI и QPI, работающих
 * при напряжениях питания и сигналов в 3,3 В.
 * 
 * Также у всех вышеперечисленных микросхем есть режим Performance Enhance Mode
 * - XIP (execute-in-place), который позволяет при каждой последующей операции
 * не отправлять повторно ту же самую команду. Это экономит 8 тактов команды в
 * QSPI режиме и 2 такта команды в QPI режиме. W25Q64FV работает без проблем,
 * у остальных должно работать точно также, судя по документации. Для этого
 * нужно выполнить команду чтения любого адреса, где в промежуточных данных
 * после него нужно отправить 0x20. Затем переключаем SPIFI в режим без кода
 * операции, где только 3 байта адреса в запросе плюс промежуточный байт данных
 * 0x20. Для других типов микросхем, например EN25Q80B, этот байт должен иметь
 * исключающие друг друга полубайты, например: 0xA5, 0x5A, 0x0F, 0xF0 и т.п.
 * У самых дешёвых микросхем время ожидания вывода данных даже на относительно
 * низких тактовых частотах может составлять от четырёх до шести тактов (2—3
 * байта), а иногда и больше.
 *  
 *  Варианты настройки константы "CHIP_MODE":
 * 
 *  0 = Автовыбор по итогу проверки связи: [SPI]->[QDO]->[QIO]->[QPI]->[XIP]
 *  1 = SPI — Весь обмен по одной линии связи. Это самый медленный вариант.
 *  2 = QDO — По четырём линиям связи передаются только данные.
 *  3 = QIO — По четырём линиям не передаются только команды.
 *  4 = QPI — Весь обмен осуществляется в четырёхпроводном режиме.
 *  5 = XIP — Весь обмен четырёхпроводной, но вообще без передачи команд.
 * 
 *  Любое значение вне диапазона чисел от 0 до 5 будет восприниматься как 0.
 * 
 * В зависимости от выбора в программе подключаются/исключаются те или иные части
 * кода, что естественно изменяет размер получаемой из него прошивки.
 * 
 * Рекомендуется использовать режим XIP, при котором задержка выборки данных из
 * внешней МФП минимальна, что особенно важно при выполнении из неё программ.
 * 
 *            XIP — выполнение на месте (eXecute-In-Place).
 */
