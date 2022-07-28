
// #include "common.h"
// #include "spi_loop_common.h"


// int main()
// {
//     SystemInit(); 
    
//     PM->CLK_APB_P_SET = PM_CLOCK_SPI_0_M | PM_CLOCK_SPI_1_M;
//     PM->CLK_APB_M_SET = PM_CLOCK_EPIC_M;

//     //проверка ID 
//     if (!TestSpiId(SPI_0))
//     {
//         return 0;
//     }
        
//     if (!TestSpiId(SPI_1))
//     {   
//         return 0;
//     }

//     xprintf("\nSPI_0 - master; SPI_1 - slave\n");
//     TestExchange(SPI_0, SPI_1, EPIC_SPI_1_INDEX);
//     xprintf("\nSPI1 - master; SPI_0 - slave\n");
//     TestExchange(SPI_1, SPI_0, EPIC_SPI_0_INDEX);

// 	exit ();
// }


