#include "mik32_hal_pcc.h"
#include "mik32_hal_spifi_w25.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "riscv_csr_encoding.h"
#include "csr.h"

/*
 * Данный пример демонстрирует настройку и переход в программу из SPIFI.
 * В примере включается тактирование SPIFI и настраиваются выводы, включается кэширование
 * и настраивается адрес входа прерывание.
 */

static void Scr1_Timer_Init(void);

void SystemClock_Config();
void SPIFI_Init();


int main()
{
    SystemClock_Config();

    SPIFI_Init();

    write_csr(mtvec, 0x80000000); // Задать адрес входа в прерывание как 0x800000C0

    asm volatile("la ra, 0x80000000\n\t"
                 "jalr ra");

    while (1)
        ;
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

void SPIFI_Init()
{
    SPIFI_HandleTypeDef spifi = {.Instance = SPIFI_CONFIG};

    HAL_SPIFI_MspInit(&spifi);
    HAL_SPIFI_Reset(&spifi);

    uint8_t sreg1 = HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG1);
    uint8_t sreg2 = HAL_SPIFI_W25_ReadSREG(&spifi, W25_SREG2);

    HAL_SPIFI_W25_WriteSREG(&spifi, sreg1, sreg2 | (1 << 1));

    SPIFI_MemoryCommandTypeDef cmd_mem = {
        .OpCode = 0xEB,
        .FieldForm = SPIFI_CONFIG_CMD_FIELDFORM_OPCODE_SERIAL,
        .FrameForm = SPIFI_CONFIG_CMD_FRAMEFORM_OPCODE_3ADDR,
        .InterimData = 0,
        .InterimLength = 3,
    };

    SPIFI_MemoryModeConfig_HandleTypeDef spifi_mem = {
        .Instance = spifi.Instance,
        .CacheEnable = SPIFI_CACHE_ENABLE,
        .CacheLimit = 0x90000000,
        .Command = cmd_mem,
    };

    HAL_SPIFI_MemoryMode_Init(&spifi_mem);
}
