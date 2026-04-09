/*
 * system_states.c
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#include"system_states.h"
 SystemStates current_state =INIT;
int count=0,pid_time_count,obc_hb_count,brake_initial_count;
bool obj_detection,brake_initial_check,brake_data_send,steering_check,pid_enable,mcu_data_check,motor_check_on,mcu_start_tmr_check=0,run_flag=1,motor_state,hvac_state,dc_dc_state,charger_contactor_state,state_status,send_data_to_pdu_flag=0;//can1_check=0,
char tx_buff[128];
//uint8_t PDU_data[8]={0x01,0x00,0x01,0x01,0x00,0x00,0x00,0x00};
uint8_t Critical_System_Status,vcu_id=1,obc_id=1,Passenger_load,rpm,obj_dect,Check_Sum,send_to_bms=4,feedback_from_pdu,data_byte;

void diagnosis(void){

	if(!(pdu.error&&mcu.error&&bms.error && brake.error)){//&&error_in_imd&&error_in_dc_dc&&error_in_eps){
		diagnosis_success=1;
	}


}

void sending_data_to_bms(){
							bms.tx_data[0]=send_to_bms;
							bms.tx_data[1]=0;
							bms.tx_data[2]=0;
							bms.tx_data[3]=0;
							bms.tx_data[4]=0;
							bms.tx_data[5]=0;
							bms.tx_data[6]=0;
							bms.tx_data[7]=0;

							FLEXCAN_DRV_Send(INST_CANCOM1,8U,&can1_data_init,to_bms,bms.tx_data);///to bms
}
void sending_data_to_pdu(){
							pdu.tx_data[0]=motor_state;//1 on
							pdu.tx_data[1]=hvac_state;
							pdu.tx_data[2]=dc_dc_state;
							pdu.tx_data[3]=charger_contactor_state;
							pdu.tx_data[4]=0;
							pdu.tx_data[5]=0;
							pdu.tx_data[6]=0;
							pdu.tx_data[7]=0;
      feedback_from_pdu=motor_state|hvac_state<<1|dc_dc_state<<2|charger_contactor_state<<3;
      FLEXCAN_DRV_Send(INST_CANCOM1,9U,&can1_data_init,to_pdu,pdu.tx_data);///to PDU
			 if(feedback_from_pdu==pdu.data[0]){
				send_data_to_pdu_flag=0;
                motor_check_on=pdu.data[0] & 0x01;//1
                brake_initial_check=pdu.data[0] & 0x04;
			 }


}
void sending_data_to_obc(){
							obc.tx_data[0]=current_state;
							obc.tx_data[1]=state_status;
							obc.tx_data[2]=Critical_System_Status;
							obc.tx_data[3]=rfid_tag;
							obc.tx_data[4]=obj_dect ;
							obc.tx_data[5]=rpm;
							obc.tx_data[6]=obc.ack;
							obc.tx_data[7]=obc.check_sum;

    FLEXCAN_DRV_Send(INST_CANCOM2,9U,&can2_data_init,to_obc,obc.tx_data);///to obc

}

void sending_data_to_obc_heart_beat(){
							obc.tx_data[0]=vcu_id;
							obc.tx_data[1]=obc_id;
							obc.tx_data[2]=0;
							obc.tx_data[3]=0;
							obc.tx_data[4]=0 ;
							obc.tx_data[5]=0;
							obc.tx_data[6]=0;
							obc.tx_data[7]=0;

    FLEXCAN_DRV_Send(INST_CANCOM2,11U,&can2_data_init,to_obc_hb,obc.tx_data);///to obc
    FLEXCAN_DRV_Send(INST_CANCOM1,11U,&can1_data_init,to_obc_hb,obc.tx_data);///to all ECU's
}


void system_states(){
	switch(current_state){
					case INIT:
					{
						if(uart_check&&clock_check&&pin_check&&can0.init_success&&can1.init_success&&can2.init_success && spi0.init_success)
						{
							//count++;
						sprintf(tx_buff,"Initialization\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));

						//send_to_bms=4;
						sending_data_to_bms();
						state_status=0;
						throttle_relay_off;
						//sending_data_to_obc();

						if(bms.data[0]==5)
							{
								bms.on=1;
								state_status=1;
								current_state=WAKE_UP;
								sending_data_to_obc();
							}
						}
						break;
					}
					case WAKE_UP:
					{

						if(pdu.on==0){
							motor_state=1,dc_dc_state=1;
							send_data_to_pdu_flag=1;
							OSIF_TimeDelay(100);
							state_status=0;
							//sending_data_to_obc();
							if(pdu.data[0]==5){
							   pdu.on=1;

							}

						}
						else if(pdu.on==1){

							press_bar=30;

							if(applied_press>=25){
								brake.on=1;
							}
						}
						if(pdu.on && mcu.on&& bms.on && brake.on ){
							wake_up_success=1;
						}
						if(wake_up_success==1){
							//  FLEXCAN_DRV_Send(INST_CANCOM2,2U,&can2_data_init, 0x180E4A2,can_data_1);
							state_status=1;

						}

						sprintf(tx_buff,"Wake_up\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case DIAGNOSTIC:
					{
						diagnosis_on=1;
						diagnosis();
						state_status=0;
						if(diagnosis_success==1){
							// FLEXCAN_DRV_Send(INST_CANCOM2,2U,&can2_data_init, 0x180E4B1,can_data_1);
							state_status=1;
							//current_state=STOP;
							diagnosis_on=0;
						}
						sprintf(tx_buff,"Diagnostic\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case STOP :
					{
						//PINS_DRV_ClearPins(PTB,1<<10);//MOTOR
						motor_state=0;
						mcu_start_tmr_check=0;
						send_data_to_pdu_flag=1;
						pid_enable=0;
						press_bar=30;
						state_status=0;
						throttle_relay_off;
						TPID.OutputSum=0;
						Output=0;
						MCP4921_SetOutput((uint16_t)Output);


					//	if(applied_press<=press_bar){
							if(data_byte==3){
								OSIF_TimeDelay(4000);
								run_flag=1;
								current_state=RUNNING;
							}
							else if(data_byte==0){
								state_status=1;
								data_byte=0;
							}
					//	}

						sprintf(tx_buff,"Stop\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case RUNNING :
					{
						//i=1;//start throttle contrlo

						if(run_flag == 1){

							state_status=0;
							motor_state=1;
							send_data_to_pdu_flag=1;
//							OSIF_TimeDelay(2000);

							press_bar=0;

							OSIF_TimeDelay(4000);
							run_flag=0;

						}
						if(obc.data[data_byte]=='F'){

						//	if(applied_press<=press_bar){
							if(steering_check==1){
								forward_relay;
								throttle_relay_on;
								pid_enable=1;
							}
//								if(obj_detection==1){
//
//									pid_enable=0;
//									press_bar=30;
//									throttle_relay_off;
//									TPID.OutputSum=0;
//									Output=0;
//									MCP4921_SetOutput((uint16_t)Output);
//
//								}
//								else if(obj_detection==0){
//									press_bar=0;
//									forward_relay;
//									pid_enable=1;
//									throttle_relay_on;
//								}


								if(rfid_tag=='B'&&obc.data[3]=='R'){
									data_byte=3;
									current_state=STOP;
									state_status=1;
								}
								else if(rfid_tag=='B'&&obc.data[3]=='0'){
									data_byte=0;
									current_state=STOP;
									state_status=1;
								}
								else if(rfid_tag=='B'&&obc.data[3]=='F'){
									data_byte=0;
									current_state=STOP;
									state_status=1;
								}
							//}
						}
						else if(obc.data[data_byte]=='R'){

							//if(applied_press<=press_bar){
							if(steering_check==1){
								reverse_relay;
								throttle_relay_on;
								pid_enable=1;
							}
								if(rfid_tag=='A'&&obc.data[3]=='F'){
									data_byte=3;
									current_state=STOP;
									state_status=1;
								}
								else if(rfid_tag=='A'&&obc.data[3]=='0'){
									data_byte=0;
									current_state=STOP;
									state_status=1;
								}
								else if(rfid_tag=='A'&&obc.data[3]=='R'){
									data_byte=0;
									current_state=STOP;
									state_status=1;
								}
							}
						//}



//						sprintf(tx_buff,"Running\r\n");
//					    LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
					    break;
					}
					case DOOR_OPEN :
						{
							//SEND CAN DATA
							sprintf(tx_buff,"Door Open\r\n");
							LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
							break;
						}
					case DOOR_CLOSE :
						{
							//SEND CAN DATA
							sprintf(tx_buff,"Door Close\r\n");
							LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
							break;
						}
					case SLEEP :
						{
							sprintf(tx_buff,"Sleep\r\n");
							LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						}
					case DEEP_SLEEP :
						{
							sprintf(tx_buff,"Deep Sleep\r\n");
							LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
							break;
						}
					case CHARGE :
						{
							sprintf(tx_buff,"Charge\r\n");
							LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
							break;
							//SEND CAN DATA
						}

					case FAULT:
					{
						motor_state=0;
						send_data_to_pdu_flag=1;
						pid_enable=0;
						press_bar=30;
						state_status=0;
						throttle_relay_off;
						TPID.OutputSum=0;
						Output=0;
						MCP4921_SetOutput((uint16_t)Output);

						sprintf(tx_buff,"Fault\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case EMERGENCY:
					{
						sprintf(tx_buff,"Emergency\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
				}
}



