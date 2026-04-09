/*
 * system_states.c
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#include"system_states.h"
 SystemStates current_state =INIT;
uint32_t count=0,pid_time_count,obc_hb_count_tx,all_ecu_hb_count_tx,brake_initial_count,obc_hb_count_rx;
bool trip_status,brake_initial_check,hb_check_obc=0,brake_data_send,steering_check,pid_enable,fault_checks_enable,motor_check_on,mcu_start_tmr_check=0,run_flag=1,stop_flag=1,motor_state,hvac_state,dc_dc_state,charger_contactor_state,state_status,start_pdu_check,send_data_to_pdu_flag=0;//can1_check=0,
char tx_buff[128];
//uint8_t PDU_data[8]={0x01,0x00,0x01,0x01,0x00,0x00,0x00,0x00};
uint8_t  trip_data_buf[8],trip_ack_rx_buf[8],Critical_System_Status,vcu_id=1,obc_id=1,Pay_load,rpm,obj_dect,Check_Sum,send_to_bms=4,feedback_from_pdu,data_byte,obj_detection;

void diagnosis(void){

	if(!(pdu.fault_alarm && mcu.fault_alarm && bms.fault_alarm && brake.fault_alarm)){//To Confirm there is no fault in the all the ECU//&&fault_alarm_in_imd&&fault_alarm_in_dc_dc&&fault_alarm_in_eps){
		diagnosis_success=1;
	}


}

void sending_data_to_bms(){
							bms.tx_data[0]=send_to_bms;//sleep,wake up ,discharge
							bms.tx_data[1]=0;
							bms.tx_data[2]=0;
							bms.tx_data[3]=0;
							bms.tx_data[4]=0;
							bms.tx_data[5]=0;
							bms.tx_data[6]=0;
							bms.tx_data[7]=0;

							FLEXCAN_DRV_Send(INST_CANCOM1,8U,&can1_data_init,to_bms,bms.tx_data);///to bms
}
void sending_data_to_pdu(){//to send data to PDU
							pdu.tx_data[0]=motor_state;
							pdu.tx_data[1]=hvac_state;
							pdu.tx_data[2]=dc_dc_state;
							pdu.tx_data[3]=charger_contactor_state;
							pdu.tx_data[4]=0;
							pdu.tx_data[5]=0;
							pdu.tx_data[6]=0;
							pdu.tx_data[7]=0;
      feedback_from_pdu=motor_state|hvac_state<<1|dc_dc_state<<2|charger_contactor_state<<3;
      FLEXCAN_DRV_Send(INST_CANCOM1,9U,&can1_data_init,to_pdu,pdu.tx_data);///to PDU
			 if(feedback_from_pdu==pdu.data[0]){//to get feedback confirmation from PDU
				send_data_to_pdu_flag=0;//this is stop flag to stop continuous msg that is sent to PDU
                motor_check_on=pdu.data[0] & 0x01;//motor_check_on is used to store the motor controller ON/OFF feedback from the PDU
                brake_initial_check=pdu.data[0] & 0x04;//brake_initial_check  is used to store the DC-DC ON/OFF feedback from the PDU
			 }
			 pdu.count_to_send_data=0;//reset the counter after 10 counts i.e 100ms
}

void sending_data_to_all_ecu_heart_beat(){
							all_ecu.tx_data[0]=vcu_id;
							all_ecu.tx_data[1]=0;
							all_ecu.tx_data[2]=0;
							all_ecu.tx_data[3]=0;
							all_ecu.tx_data[4]=0;
							all_ecu.tx_data[5]=0;
							all_ecu.tx_data[6]=0;
							all_ecu.tx_data[7]=0;


    FLEXCAN_DRV_Send(INST_CANCOM1,14U,&can1_data_init,to_all_ecu_hb,all_ecu.tx_data);///to all ECU's
}

void system_states(){
	switch(current_state){
					case INIT:
					{
						if(uart_check&&clock_check&&pin_check&&can0.init_success&&can1.init_success&&can2.init_success && spi0.init_success)//it checks all the peripherals are initialized or not
						{
							//count++;
						sprintf(tx_buff,"Initialization\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));

						//send_to_bms=
						sending_data_to_bms();//after Initializing sending msg to Bms to Discharge State
						state_status=0;
						throttle_relay_off;
						//sending_data_to_obc();

						if(bms.data[0]==5)//if BMS is in Discharge State Then System goes to Wake up state
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

						if(pdu.on==0){//initially PDU is Off
							//OSIF_TimeDelay(2000);
							motor_state=1,dc_dc_state=1;//turning ON Motor Controller and DC-DC  converter
							send_data_to_pdu_flag=1;//this is stop/start flag to stop/start continuous msg that is sent to PDU
							OSIF_TimeDelay(100);
							state_status=0;
							//sending_data_to_obc();
							if(pdu.data[0]==5){//if feedback matches as Expected the Assuming  PDU is ON
							   pdu.on=1;
							}

						}
						else if((pdu.on==1)&&(wake_up_success==0)){

							press_bar=30;//Applying Brake ,brake function is continuously running in 10ms Timer interrupt
							press_applied=press_bar;
							if(applied_press>=25){//if Brake Applied Successful the considering Brake is ON
								brake.on=1;
							}
						}
						if(pdu.on && mcu.on&& bms.on && brake.on ){//to confirm all the ECU's are ON
							wake_up_success=1;//used to check fault_check after wake-up
						}
						if(wake_up_success==1){
							//  FLEXCAN_DRV_Send(INST_CANCOM2,2U,&can2_data_init, 0x180E4A2,can_data_1);
							state_status=1;

						}

//						sprintf(tx_buff,"Wake_up\r\n");
//						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case DIAGNOSTIC:
					{
						diagnosis_on=1;
						diagnosis();

						if(diagnosis_success==1){
							// FLEXCAN_DRV_Send(INST_CANCOM2,2U,&can2_data_init, 0x180E4B1,can_data_1);
							state_status=1;
							//current_state=STOP;
							diagnosis_on=0;
							fault_checks_enable=1;//to continue the fault check in the system ,recovery is not yet implemented
						}
						sprintf(tx_buff,"Diagnostic\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case STOP :
					{
						//PINS_DRV_ClearPins(PTB,1<<10);//MOTOR
					if(stop_flag==1){//until the stop state is success

						throttle_relay_off;//throttle control relay
						TPID.OutputSum=0;//making PID output to zero
						Output=0;
						MCP4921_SetOutput((uint16_t)Output);
						pid_enable=0;//Disabling the PID Flag
						if(!temp_brake_release){//use to release brake via switch
							press_bar=40;
							press_applied=press_bar;
						}
						state_status=0;
						if(mcu.rpm<=10){//to check Motor RPM is Less than 10
						motor_state=0;
						send_data_to_pdu_flag=1;
						}
					}
					if(stop_check()){
						stop_flag=0;
							if(data_byte==3){//for trip

								OSIF_TimeDelay(4000);//it will stop for 4sec and Start the next Process
								run_flag=1;
								current_state=RUNNING;
							}
							else if(data_byte==0){//this is for Trip end

								if(state_status==0){
									trip_status_send=1;//flag to send the trip status
									trip_status=0;//0-trip  end ,1-in trip
									state_status=1;
								}



							}
						//}
					}

						sprintf(tx_buff,"Stop\r\n");
						LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
						break;
					}
					case RUNNING :
					{
						if(door_status())//only if door is closed vehicle starts running else  it goes to fault state
						{
							over_speed_check();//rpm cutoff 300
							//i=1;//start throttle control

							if(run_flag == 1){//to reset and start the System
								TPID.OutputSum=0;
								Output=0;
								MCP4921_SetOutput((uint16_t)Output);
								pid_enable=0;
								press_bar=30;
								throttle_relay_off;

								state_status=0;
								motor_state=1;
								press_applied=0;
								send_data_to_pdu_flag=1;//flag to send data to PDU
	//							OSIF_TimeDelay(2000);
								trip_status=1;
								//press_bar=0;
								OSIF_TimeDelay(4000);

								run_flag=0;
							}
							else{
							                            //door_status();//door_status check

							    if(obc.data[data_byte]=='F'){


									   if(radar_detection_check()<=2){//to check the collision only in Forward
											if(ready_to_start()){
												//Setpoint=750;
												forward_relay;
												throttle_relay_on;
												//OSIF_TimeDelay(10);
												pid_enable=1;
												//press_bar=0;
												sprintf(tx_buff,"in forward\r\n");
												LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
											}
										}

										if(rfid_tag=='B'&&obc.data[3]=='R'){
											data_byte=3;
											current_state=STOP;
											state_status=1;
											stop_flag=1;
										}
										else if(rfid_tag=='B'&&obc.data[3]=='0'){
											data_byte=0;
											current_state=STOP;
											state_status=1;
											stop_flag=1;
										}
										else if(rfid_tag=='B'&&obc.data[3]=='F'){
											data_byte=0;
											current_state=STOP;
											state_status=1;
											stop_flag=1;
											//press_bar=0;
										}

								}
								else if(obc.data[data_byte]=='R'){


								    Setpoint=200;
									if(ready_to_start()){
										reverse_relay;
										throttle_relay_on;
										//OSIF_TimeDelay(10);
										pid_enable=1;
										sprintf(tx_buff,"in reverse\r\n");
										LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
									}
										if(rfid_tag=='A'&&obc.data[3]=='F'){
											data_byte=3;
											current_state=STOP;
											state_status=1;
											stop_flag=1;
										}
										else if(rfid_tag=='A'&&obc.data[3]=='0'){
											data_byte=0;
											current_state=STOP;
											state_status=1;
											stop_flag=1;
										}
										else if(rfid_tag=='A'&&obc.data[3]=='R'){
											data_byte=0;
											current_state=STOP;
											state_status=1;
											stop_flag=1;
										}
									}
							 }
						}
						else
						{
							current_state=FAULT;
						}



//						sprintf(tx_buff,"Running\r\n");
//					    LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
					    break;
					}
					case DOOR_OPEN :
						{
							//SEND CAN DATA
							if(stop_check() && door_status()==0){//it check the Vehicle is in Stop State and then Door opens
								if(state_status==0){
									trip_status_send=1;
								}
								state_status=1;
							}
							else{
								state_status=0;
							}

							sprintf(tx_buff,"Door Open\r\n");
							LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));
							break;
						}
					case DOOR_CLOSE :
						{
							//SEND CAN DATA
							if(door_status()){//currently there is no command to Open/Close the Door to send to the ECU
								if(state_status==0){
									trip_status_send=1;
								}
								state_status=1;
							}
							else{
								state_status=0;
							}

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
						if(!temp_brake_release){
							press_bar=50;
							press_applied=press_bar;
						}
						state_status=0;
						throttle_relay_off;
						TPID.OutputSum=0;
						Output=0;
						MCP4921_SetOutput((uint16_t)Output);
						//if(mcu.can_fault || pdu.can_fault || radar.can_fault || brake.can_fault)
						fault_can_mesg();
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



