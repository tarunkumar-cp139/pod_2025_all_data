/*
 * lptimer.c
 *
 *  Created on: 04-Dec-2025
 *      Author: Tarun
 */
#include "lptimer.h"
void timer_for_10ms(){
	if(brake_initial_check){
		  brake_initial_count++;
		if(brake_initial_count>=300){//initially wait for 3sec

		  brake_initial_count=0;
		  brake_initial_check=0;
		  brake_data_send=1;
		}
	}

	 if(brake_data_send){

			 sending_data_to_brake();

	 }
  if(send_data_to_pdu_flag==1){
		pdu.count++;
	if(pdu.count>=10){
		sending_data_to_pdu();
		pdu.count=0;
	}
  }

  obc.count++;
  if( obc.count>=10){//100 ms
	sending_data_to_obc();
	 obc.count=0;
  }
  if(current_state>=3){//The heart Beat to OBC should start sending  & receiving check to be done in Stop State after Successful Diagnostic state.
	  obc_hb_count++;
	  if(obc_hb_count>=100){//1sec
		  sending_data_to_obc_heart_beat();
		  obc_hb_count=0;

	  }
  }
  if(pid_enable==1){
	pid_time_count++;
	  if(pid_time_count>=10){
//		  if(Motor_rpm)
//		  {
 	        Input=mcu.rpm;
 	 		PID_Compute(&TPID);
 	 	//	Output=680;
 	 		MCP4921_SetOutput((uint16_t)Output);
 	 		Motor_rpm=false;
		 // }
		  snprintf(tx_buff, sizeof(tx_buff), "Output: %d,motor_rpm :%d , speed : %d ,kp : %d ,ki : %d ,kd : %d ,setpoint :  %d\r\n", (uint16_t)Output,(uint16_t)mcu.rpm,(uint16_t)(mcu.speed*10),(uint16_t)(consKp*10),(uint16_t)(consKi*10),(uint16_t)(consKd*10),(uint16_t)Setpoint);

		  //snprintf(tx_buff, sizeof(tx_buff), "Output: %.2f,motor_rpm :%.2f, speed : %.2f ,kp : %.3f ,ki : %.3f ,kd : %.3f ,setpoint :  %.2f\r\n",Output,mcu.rpm,(mcu.speed),(consKp),(consKi),(consKd),Setpoint);
		  		LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
 	 		//press_bar=0;
 	 		pid_time_count=0;
	     }
 	  }
  else if(pid_enable==0){
		TPID.OutputSum=0;
		Output=0;
		MCP4921_SetOutput((uint16_t)Output);

  }

if(motor_state && motor_check_on){

	  mcu.count++;
	  if(mcu.count>=500){// initial check
		  if(mcu_data_check==0){
			    current_state =FAULT;
			    sprintf(tx_buff,"5sec fault\r\n");
			    LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
			    mcu_start_tmr_check=0;
		  }
		  else if(mcu_data_check==1){
			  mcu_start_tmr_check=1;
		  }
	      mcu_data_check=0;
	  	  mcu.count=0;

	  }
		if(motor_state && motor_check_on && mcu_start_tmr_check){
			  if(mcu.count>=300){
					if(mcu_data_check==0){
						current_state =FAULT;
						sprintf(tx_buff,"600ms fault\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
					}
					mcu_data_check=0;
					mcu.count=0;
			  }
		}
}



 LPTMR_DRV_ClearCompareFlag(INST_LPTMR1);
}
void timer_config(){
  	LPTMR_DRV_Init(INST_LPTMR1, &lpTmr1_config0,1);
  	INT_SYS_InstallHandler(LPTMR0_IRQn,timer_for_10ms,NULL);

  	INT_SYS_EnableIRQ(LPTMR0_IRQn);

  	LPTMR_DRV_StartCounter(INST_LPTMR1);
  	LPTMR_DRV_SetInterrupt(INST_LPTMR1,1);


  }
