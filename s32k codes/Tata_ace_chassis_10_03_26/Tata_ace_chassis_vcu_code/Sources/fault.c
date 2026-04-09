/*
 * fault.c
 *
 *  Created on: 07-Jan-2026
 *      Author: Tarun
 */


#include "fault.h"

parameter fault;
bool fault_can_mesg(void){//to indicate the fault sends to OBC
	//if(fault_checks_enable==1){
	         	fault.tx_data[0] =radar.can_fault<<5 | obc.can_fault<<4 | bms.can_fault<<3 | brake.can_fault<<2 | pdu.can_fault <<1 | mcu.can_fault ;//if the ECU CAN communication failed for a perticular time period can_fault will trigger
		if(wake_up_success){
				fault.tx_data[1] =obc.fault_alarm<<4 | bms.fault_alarm<<3 | brake.fault_alarm<<2 | pdu.fault_alarm <<1 | mcu.fault_alarm ;//fault detected by the in-buit ECU and informed to VCU
				fault.tx_data[2] = mcu.can_fault ;//mcu.data[8];
				fault.tx_data[3] = pdu.can_fault;//pdu.data[7];
				fault.tx_data[4] = brake.can_fault;//(brake.data[2]>>6) & (0x03);
				fault.tx_data[5] = radar.can_fault;// bms.data[7];
				fault.tx_data[6] = obc.data[8];
				fault.tx_data[7] = fault.tx_data[0] + fault.tx_data[1] +  fault.tx_data[2] +fault.tx_data[3] + fault.tx_data[4] + fault.tx_data[5] +fault.tx_data[6];
		}
		if((fault.tx_data[0] >=1 ) || (fault.tx_data[1] >=1 )){
			//FLEXCAN_DRV_Send(INST_CANCOM1,15U,&can1_data_init,VCU_fault,fault.tx_data);
			  FLEXCAN_DRV_Send(INST_CANCOM2,8U,&can2_data_init,VCU_fault,fault.tx_data);///to obc
			return 1;
		}
		else {
			return 0;
		}
	//}
}
/* rest of the ECUs
bool fault_can_mesg1(void){
	fault.tx_data[0] = eps.fault_alarm<<4 | charger.fault_alarm<<3 | loadcell.fault_alarm<<2 | navigation_ecu.fault_alarm <<1 | hvac.fault_alarm ;
	fault.tx_data[1] = hvac.data[8];
	fault.tx_data[2] = navigation_ecu.data[7];
	fault.tx_data[3] = loadcell[7];
	fault.tx_data[4] = charger.data[7];
	fault.tx_data[5] = eps.data[8];
	fault.tx_data[6] = 0;
	fault.tx_data[7] = fault.tx_data[0] + fault.tx_data[1] +  fault.tx_data[2] +fault.tx_data[3] + fault.tx_data[4] + fault.tx_data[5] +fault.tx_data[6];

	if(fault.tx_data[0] >=1 ){
		FLEXCAN_DRV_Send(INST_CANCOM1,8U,&can1_data_init,0x18000013,fault.tx_data);
		return 1;
	}
	else {
		return 0;
	}
}
*/
