/*
 * apm_std.c
 *
 *  Created on: 06-Jan-2026
 *      Author: Tarun
 */


#include "apm_std.h"

volatile  uint32_t flag=0;

bool intr_triger=0,temp_brake_release=0;
bool pinstate =0,pinstate2 =0,door_close=1;

void gpio_intrr_config(){//interrupts for emergency button AND Brakes Release
	PINS_DRV_SetPinIntSel(PORTC,17,PORT_INT_RISING_EDGE);
	PINS_DRV_SetPinIntSel(PORTC,9,PORT_INT_RISING_EDGE);
		INT_SYS_InstallHandler(PORTC_IRQn,gpio_int,NULL);
		INT_SYS_EnableIRQ(PORTC_IRQn);
}

void gpio_int(){

	    	flag = PINS_DRV_ReadPins(PTC);

	    	pinstate = (flag >> 17)&(0x1);//this is used to check emergency button
	    	pinstate2 = (flag >> 9)&(0x1);//this is to release the brakes
	    	if(pinstate){

	    		//current_state =FAULT;

				rfid_tag=0;

	    		intr_triger=1;
	    		current_state =FAULT;
	    		PINS_DRV_ClearPinIntFlagCmd(PORTC,17);
	    	}
	    	if(pinstate2){

	    		temp_brake_release=1;

	    		PINS_DRV_ClearPinIntFlagCmd(PORTC,9);
	    	}
	    	PINS_DRV_ClearPinIntFlagCmd(PORTC,9);
	    	PINS_DRV_ClearPinIntFlagCmd(PORTC,17);

}

//void apm_std_check(int condition_check){
//
//
//}

bool ready_to_start(void){//to check the parameters
	if(brake_released() && (steering_check==1) && motor_check_on){
		return 1;
	}
	else{
		return 0;
	}
}
bool stop_check(void){//to confirm the Stop as per APM Standards
	if( motor_check_on==0 && applied_press >=25 && mcu.rpm <=10)
		return 1;
	else
		return 0;
}

bool door_status(void){//to check the Door Status from Door ECU

if(door_close)//there is no command implemented to open / close the door
  return 1;//close
else
  return 0;//open
}

bool over_speed_check(void){
	if(mcu.rpm>=1500){//this Should be Modified as you increase the Setpoint ,it should be setpoint+100
			current_state =FAULT;//STOP
			return 1;
		}
	else
		return 0;
}

uint8_t radar_detection_check(void){//TO take actions Based on the obj in the Range


	if(obj_detection==0){

		Setpoint=set_rpm;//set_rpm is default set to 750 ,can be modified by sending rpm in CAN msg (little endian format)
		return 0;

	}
	else if(obj_detection==1){
		//critical gpio
		return 1;

	}
	else if(obj_detection==2){

		// relay warning
		Setpoint=(set_rpm/2);//rpm to half
		return 2;

	}
	else if(obj_detection==3){

		throttle_relay_off;
		TPID.OutputSum=0;
		Output=0;
		MCP4921_SetOutput((uint16_t)Output);
		pid_enable=0;
		if(!temp_brake_release){
			press_bar=30;
			press_applied=press_bar;
		}// relay caution
		return 3;
	}


}

void all_ecu_fault_check(void){///here motor And PDU  can data is being checked currently

	//motor CAN Data check
	if(motor_state && motor_check_on){
		  mcu.count_fault_check++;
		  if(mcu.count_fault_check>=500){//initial check 5sec
				if(mcu.data_check==0){

					mcu.can_fault=1;
					current_state =FAULT;
//					sprintf(tx_buff,"5sec fault\r\n");
//					LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
					mcu_start_tmr_check=0;

				}
				 else if(mcu.data_check==1){
							  mcu_start_tmr_check=1;
						  }
				mcu.data_check=0;
				mcu.count_fault_check=0;

		  }
		  if(motor_state && motor_check_on && mcu_start_tmr_check){
		  			  if(mcu.count_fault_check>=200){//2sec

		  					if(mcu.data_check==0){
		  						mcu.can_fault=1;
		  						current_state =FAULT;
//		  						sprintf(tx_buff,"1s fault\r\n");
//		  						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
		  					}
		  					mcu.data_check=0;
		  					mcu.count_fault_check=0;
		  			  }
		  		}
	}

	//PDU CAN Data check

//	if((current_state>=1) && (send_data_to_pdu_flag==1)){
//		start_pdu_check=1;
//	}

	pdu.count_fault_check++;
	if(current_state>=2){//check Starts from Diagnosis state
		if(pdu.count_fault_check>=400){//every 4ec
			if(pdu.data_check==0){
				pdu.can_fault=1;
				current_state =FAULT;
			}

			if(radar.data_check==0) //check radar fault
			{
				radar.can_fault=1;
				current_state =FAULT;
			}
			radar.data_check=0;
			pdu.data_check=0;
		    pdu.count_fault_check=0;
		}

	}

	////Brake CAN Data check
	/*
	brake.count_fault_check++;
	if((current_state>=1 && pdu.on==1)) {
		if(brake.count_fault_check>=25){//every 250ms
				brake.can_fault=1;
				current_state =FAULT;

			}
		brake.data_check=0;
		brake.count_fault_check=0;
		}
   */
}


