/*
 * lptimer.c
 *
 *  Created on: 04-Dec-2025
 *      Author: Tarun
 */
#include "lptimer.h"
uint8_t counter_for_1sec;
bool led_blink=0;
void timer_for_10ms(){//10 ms timer is used for Sending the message to brakes for every 10ms
//  sending_data_to_brake();
  if(brake_initial_check){//for the first Start the brake will take some time to start
  		  brake_initial_count++;
  		if(brake_initial_count>=400){//initially wait for 4sec

  		  brake_initial_count=0;
  		  brake_initial_check=0;
  		  brake_data_send=1;
  		}
  	}
  	if(brake_data_send){
  		 sending_data_to_brake();
  	}

  all_ecu_fault_check();//to check PDU ,MCU faults




  if(send_data_to_pdu_flag==1){
		pdu.count_to_send_data++;
	if(pdu.count_to_send_data==10){//to Send data to pdu
		sending_data_to_pdu();
		// pdu.count_to_send_data=0;//reset the counter after 10 counts i.e 100ms
	}
  }

  obc.count_to_send_data++;
  if( obc.count_to_send_data>=10){//to Send data to OBC for every 100 ms
	sending_data_to_obc();
	 obc.count_to_send_data=0;
	 if(trip_status_send){
	   // if(current_state>=3 ){//|| current_state==4){
		 if(!diagnosis_success)
			 send_trip_status();
		 }
	     else{
	    	 diagnosis_success=0;
	    	 trip_status_send=0;
	      }
  }
  if(current_state>=2){//The heart Beat to OBC should start sending  & receiving check to be done in Stop State after Successful Diagnostic state.
 	  obc_hb_count_tx++;
 	  if(obc_hb_count_tx>=100){//1sec

  		  sending_data_to_obc_heart_beat();
 		  sending_data_to_all_ecu_heart_beat();
 		  obc_hb_count_tx=0;



 	  }
//
 	  //to check the Received heart beat from OBC

// 	 obc_hb_count_rx++;
// 	if(obc_hb_count_rx>=1200){
// 		if(hb_check_obc==0){
// 			current_state= FAULT;
// 		}
// 		obc_hb_count_rx=0;
// 		hb_check_obc=0;
// 	}

// 	 all_ecu_hb_count_tx++;
// 	 if(all_ecu_hb_count_tx>=100){//1sec
//
// 	 	all_ecu_hb_count_tx=0;
// 	 	 }
  }

  counter_for_1sec++;
if(counter_for_1sec>=100){//led blink to check the controller is working and turning off in timer after every 1 sec
	if(led_blink==0){
		led_blink=1;
		PINS_DRV_ClearPins(PTC,1<<11);
	}
	else {
		led_blink=0;
		PINS_DRV_SetPins(PTC,1<<11);
	}



	counter_for_1sec=0;
}



  if(pid_enable==1){//to calculate PID
	pid_time_count++;
	  if(pid_time_count>=10){
		  if(Motor_rpm)
		  {
 	        Input=mcu.rpm;
 	 		PID_Compute(&TPID);
 	 	//	Output=680;
 	 		MCP4921_SetOutput((uint16_t)Output);
 	 		Motor_rpm=false;
		  }
		//  snprintf(tx_buff, sizeof(tx_buff), "Output: %d,motor_rpm :%d , speed : %d ,kp : %d ,ki : %d ,kd : %d ,setpoint :  %d\r\n", (uint16_t)Output,(uint16_t)mcu.rpm,(uint16_t)(mcu.speed*10),(uint16_t)(consKp*10),(uint16_t)(consKi*10),(uint16_t)(consKd*10),(uint16_t)Setpoint);

//		  snprintf(tx_buff, sizeof(tx_buff), "Output: %.2f,motor_rpm :%.2f, speed : %.2f ,kp : %.3f ,ki : %.3f ,kd : %.3f ,setpoint :  %.2f\r\n",Output,mcu.rpm,(mcu.speed),(consKp),(consKi),(consKd),Setpoint);
//		  		LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
 	 		//press_bar=0;
 	 		pid_time_count=0;
	     }
 	  }
    else //if(pid_enable==0){
    {
    	TPID.OutputSum=0;
		Output=0;
		MCP4921_SetOutput((uint16_t)Output);
		Motor_rpm=false;
    }




//after diagnosisnstate
//  if(current_state>=2){
//fault.count_to_send_data++;
//if(fault.count_to_send_data>=5){
//	if(fault_can_mesg());
//	fault.count_to_send_data=0;
//}
//  }
//

 LPTMR_DRV_ClearCompareFlag(INST_LPTMR1);
}
void timer_config(){
  	LPTMR_DRV_Init(INST_LPTMR1, &lpTmr1_config0,1);
  	INT_SYS_InstallHandler(LPTMR0_IRQn,timer_for_10ms,NULL);

  	INT_SYS_EnableIRQ(LPTMR0_IRQn);

  	LPTMR_DRV_StartCounter(INST_LPTMR1);
  	LPTMR_DRV_SetInterrupt(INST_LPTMR1,1);


  }
