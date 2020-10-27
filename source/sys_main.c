
//0.ducometation Section
//  29092020 code file,ADC_15_PINS_RTOS project,mainc

//  Runs on ARM TMS570LS0432 Hercules launchpad
//  INPUT to ADIN0-ADIN8 and ADIN16,ADIN17,ADIN20,ADIN21 ANALOG testing
//  Make ADIN0-ADIN8 and ADIN16,ADIN17,ADIN20,ADIN21 ANALOG as INPUT

//  Repeat this sequence of operation over and over
//  1)All ADIN Pins will be read
//  2)save the values into array
//  3)Wait 100 ms

//  Date:29.09.2020
//  Author: Mouaiad Salaas

//1.Pre-processor Directives Section
/*
* Copyright (C) 2020-20121 INOVAR R&D SOLUTIONS Company
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of INOVAR R&D SOLUTIONS Company nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* Include Files */
#include "sys_common.h"
#include "system.h"
#include "spi.h"
#include "gio.h"
//#include "het.h"
#include "reg_spi.h"
/* Include FreeRTOS scheduler files */
#include "FreeRTOS.h"
#include "os_task.h"

//1.Declarations Section
/* USER CODE BEGIN (2) */
/*  Global Variable*/
uint16 TX_Data_for_config_Master[3] = { 0x40, 0x00, 0x00};
uint16 TX_Data2_Master[3] = { 0x40, 0x09, 0xFF};

uint16 RX_Data_Master[16] = { 0 };

/* USER CODE BEGIN (1) */
/*Define Task Handles*/
xTaskHandle xTask1Handle;
/* USER CODE END */


//2.Declarations Section
/* USER CODE BEGIN (3) */

/* Task1 */
// TASK to read the ADIN PINS
// Inputs:  ALL ADIN PINS
// Outputs: None
// Notes:   None
void vTask1(void *pvParameters)
{
    spiDAT1_t dataconfig1_t;
    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL    = TRUE;
    dataconfig1_t.DFSEL   = SPI_FMT_0;
    dataconfig1_t.CSNR    = 0xFE;

    //Triggering the CS pin to 1 then 0 to start writing
    gioSetBit(spiPORT3, 0U, 1);
    gioSetBit(spiPORT3, 0U, 0);
    spiTransmitData(spiREG3, &dataconfig1_t, 8, TX_Data_for_config_Master);
    //Triggering the CS pin to 1 then 0 to start writing
    gioSetBit(spiPORT3, 0U, 1);

    while(1)
    {

       //data to send throu spi to MCP23S08
        //this data will make all output off
        TX_Data2_Master[0] = 0x40;   //this array element is device adress
        TX_Data2_Master[1] = 0x09;   //GIO Register adress
        TX_Data2_Master[2] = 0x00;   //pins of register to be off or on

        //Triggering the CS pin to 1 then 0 to start writing
        gioSetBit(spiPORT3, 0U, 1);
        gioSetBit(spiPORT3, 0U, 0);
        spiTransmitData(spiREG3, &dataconfig1_t, 8, TX_Data2_Master); //sending the data off pins mode (off or on)
        gioSetBit(spiPORT3, 0U, 1);    //change the CS pin from 0 then 1 to end writing

    }
}

/* USER CODE END */

//1.MAIN FUNCTION Section
void main(void)
{
    gioInit();
    spiInit();
    _enable_IRQ();

        /* Create Task 1 */
        if (xTaskCreate(vTask1,"Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1Handle) != pdTRUE);

        /* Start Scheduler */
        vTaskStartScheduler();
}
/* USER CODE END */

