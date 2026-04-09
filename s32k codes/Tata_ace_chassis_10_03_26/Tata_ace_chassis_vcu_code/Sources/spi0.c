/*
 * spi.c
 *
 *  Created on: 07-Dec-2025
 *      Author: Tarun
 */
#include"spi0.h"

PID_TypeDef TPID;
CanChecks spi0;
//double consKp=0.1, consKi=0.3, consKd=0.0,Input=0, Output=0,Setpoint=175;
double consKp=0.6, consKi=0.7, consKd=0.001,Input=0, Output=0,Setpoint=400;
//double consKp=0.5, consKi=0.8, consKd=0.001,Input=0.0, Output=0.0,Setpoint=200;//tata ace
CanChecks spi0 = {
     .init_check = STATUS_ERROR,
     .conf_tx_check = STATUS_ERROR,
     .conf_rx_check = STATUS_ERROR,
	  .conf_rx_check_std = STATUS_ERROR,
     .init_success = false
 };
void spi0_config(void){

	while(spi0.init_check){
        spi0.init_check=LPSPI_DRV_MasterInit(LPSPICOM1,&lpspiCom1State,&lpspiCom1_MasterConfig0);
        spi0.init_success=0;
	}
	spi0.init_success=1;
}

void pid_config(void){
	    PID(&TPID, &Input, &Output, &Setpoint, consKp, consKi, consKd, _PID_P_ON_E, _PID_CD_DIRECT);
		PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
		PID_SetSampleTime(&TPID,50);
		PID_SetOutputLimits(&TPID,600,1500);//1700
		LPSPI_DRV_SetPcs(LPSPICOM1,LPSPI_PCS0,LPSPI_ACTIVE_LOW);

		Output=0;
		MCP4921_SetOutput(Output);

}

void MCP4921_SetOutput(uint16_t dac_value)
{
    /* Limit to 12 bits */
    dac_value &= 0x0FFFU;

    /* Build control word:
       bit14 BUF=1 (buffered Vref),
       bit13 GA=1 (1x gain),
       bit12 SHDN=1 (active),
       bits[11:0] = value
    */
    dac_value &= 0x0FFFU;
    uint16_t word = (0U << 14) | (1U << 13) | (1U << 12) | dac_value;

    /* MSB first */
    uint8_t tx[2];
    tx[0] = ((word >> 8));
    tx[1] = (word & 0xFF);
    LPSPI_DRV_SetPcs(LPSPICOM1,LPSPI_PCS0,LPSPI_ACTIVE_LOW);
    /* Blocking transfer of exactly 2 bytes.
       Make sure your LPSPI component is Mode0, MSB-first, PCS0 active-low,
       and PCS continuous = true so CS stays low across both bytes. */
  // LPSPI_DRV_MasterTransferBlocking(LPSPICOM1, tx, NULL, 2U, 10U);

    LPSPI_DRV_MasterTransfer(LPSPICOM1,tx,NULL,2);
    LPSPI_DRV_SetPcs(LPSPICOM1,LPSPI_PCS0,LPSPI_ACTIVE_HIGH);
}

