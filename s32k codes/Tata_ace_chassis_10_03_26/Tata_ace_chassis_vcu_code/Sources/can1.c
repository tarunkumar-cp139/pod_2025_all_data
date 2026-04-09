/*
 * can1.c
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#include "can1.h"///data from ALL ECU'S



flexcan_data_info_t can1_data_init,can1_data_init_std;
flexcan_msgbuff_t rx_can1_init,rx_can1_init_std;
//parameter pdu,bms,mcu,brake,obc;
parameter all_ecu,radar, pdu= {.fault_alarm=true },mcu= {.fault_alarm=true },bms= {.fault_alarm=true },brake= {.fault_alarm=true },obc = {.fault_alarm=true };

bool data_from_vcu,diagnosis_success,error_in_eps,no_error_in_imd,error_in_imd,no_error_in_dc_dc,error_in_dc_dc;
bool dc_dc_on,eps_on,imd_on,wake_up_success,diagnosis_on;
uint16_t applied_press,count_fault_check,set_rpm=750;
char rfid_tag;
volatile bool Motor_rpm=false;
uint32_t mask = 0x1FFFFFFF;
CanChecks can1 = {
	      .init_check = STATUS_ERROR,
	      .conf_tx_check = STATUS_ERROR,
	      .conf_rx_check = STATUS_ERROR,
		  .conf_rx_check_std = STATUS_ERROR,
		  .conf_tx_check_std = STATUS_ERROR,
	      .init_success = false
	  };

static void can_config_rx_mb_for_extid_filter(uint8_t mb_idx, uint32_t id)
{

    // 1) Configure MB with the specific ID
   // FLEXCAN_DRV_ConfigRxMb(CAN_INSTANCE, mb_idx, &rxInfo, id);
    FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1,mb_idx,&can1_data_init, id);
    // 2) Exact-match mask: 29 ID bits + IDE
  //  uint32_t mask = 0x1FFFFFFF;    // all 29 ID bits must match
   // mask |= (1UL << 30);           // IDE bit must be 1 (extended frame)
    // mask |= (1UL << 31);        // optional: RTR must match too

    FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM1,FLEXCAN_MSG_ID_EXT , mb_idx,mask);
    FLEXCAN_DRV_Receive(INST_CANCOM1, mb_idx,&rx_can1_init);
}


void can1_config(){
	//can1.init_check=1,can1.conf_tx_check=1,can1.conf_rx_check=1;
	can1_data_init.data_length= 8;
  	can1_data_init.is_remote = false;
  	can1_data_init.msg_id_type= FLEXCAN_MSG_ID_EXT;

	can1_data_init_std=can1_data_init;
	can1_data_init_std.msg_id_type =  FLEXCAN_MSG_ID_STD;



  //	while(FLEXCAN_DRV_Init(INST_CANCOM1, &canCom1_State, &canCom1_InitConfig0)&&FLEXCAN_DRV_ConfigTxMb(INST_CANCOM1,2U,&can1_data_init,0x0)&&FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1,5U,&can1_data_init,0x0)){
  	 while(can1.init_check || can1.conf_tx_check || can1.conf_rx_check || can1.conf_rx_check_std || can1.conf_tx_check_std){
  			  can1.init_check = FLEXCAN_DRV_Init(INST_CANCOM1, &canCom1_State, &canCom1_InitConfig0);
  			  can1.conf_tx_check =  FLEXCAN_DRV_ConfigTxMb(INST_CANCOM1,2U,&can1_data_init, 0x184563f7);
  			  can1.conf_tx_check_std = FLEXCAN_DRV_ConfigTxMb(INST_CANCOM1,4U,&can1_data_init_std, 0x675);
  		//	FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM1,3U, FLEXCAN_MSG_ID_EXT, 0x1FFFFFFF);
  			  can1.conf_rx_check = FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1,3U,&can1_data_init, from_pdu);//pdu
  			  can1.conf_rx_check_std = FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1,7U,&can1_data_init_std,0x0);


  			  can1.init_success=0;
  			if(can1.count>=5){
					break;
				}
				can1.count++;
  		  }
  	 FLEXCAN_DRV_SetRxMaskType(INST_CANCOM1,FLEXCAN_RX_MASK_INDIVIDUAL );// FLEXCAN_RX_MASK_GLOBAL
  	// FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM1,3U, FLEXCAN_MSG_ID_EXT, FILTER_ID);

  ///	FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1, 3U, &can1_data_init, 0x1821FFD9);
 // 	uint32_t mask = 0x1FFFFFFF;   // 29 ID bits
    //	mask = 0x7FF; // 11 ID bit std
  	//  	mask |= (1UL << 30); ///only for ext id
  		FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM1,FLEXCAN_MSG_ID_EXT, 3U, mask);//pdu mask

  		can_config_rx_mb_for_extid_filter(4U, from_bms);
  		can_config_rx_mb_for_extid_filter(5U, from_mcu);
  		can_config_rx_mb_for_extid_filter(6U, from_steering);//
  		can_config_rx_mb_for_extid_filter(7U, from_rfid);
  		can_config_rx_mb_for_extid_filter(2U, door_status_fb);
  		can_config_rx_mb_for_extid_filter(12U, from_radar);
  		can_config_rx_mb_for_extid_filter(13U, from_radar_hb);
  		//can_config_rx_mb_for_extid_filter(15U, speed_ctrl);
  	  	FLEXCAN_DRV_InstallEventCallback(INST_CANCOM1, (flexcan_callback_t)&can1_callback, NULL);
  		FLEXCAN_DRV_Receive(INST_CANCOM1, 3U,&rx_can1_init);

//  		FLEXCAN_DRV_Receive(INST_CANCOM1, 5U,&rx_can1_init);
//  		FLEXCAN_DRV_Receive(INST_CANCOM1, 8U,&rx_can1_init);


  	////	FLEXCAN_DRV_Receive(INST_CANCOM1, 7U,&rx_can1_init_std);
  		can1.init_success=1;
}
void can1_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState)
   {
 //(void)buffIdx;
   	(void)flexcanState;

   	(void) instance;

   	switch(eventType){

   	case FLEXCAN_EVENT_RX_COMPLETE:
   	{

   		FLEXCAN_DRV_Receive(INST_CANCOM1,3U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,4U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,5U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,6U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,7U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,2U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,12U,&rx_can1_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM1,13U,&rx_can1_init);
// 		FLEXCAN_DRV_Receive(INST_CANCOM1,15U,&rx_can1_init);
   	//	FLEXCAN_DRV_Receive(INST_CANCOM3,7U,&rx_can1_init_std);


   	 		switch (rx_can1_init.msgId){


   	 		case from_pdu ://PDU
   	 		 				{

   	 		 				pdu.data_check=1;

//   	 			        pdu.data[0]=rx_can1_init.data[0];
//						pdu.data[1]=rx_can1_init.data[1];
//						pdu.data[2]=rx_can1_init.data[2];
//						pdu.data[3]=rx_can1_init.data[3];
//						pdu.data[4]=rx_can1_init.data[4];
//						pdu.data[5]=rx_can1_init.data[5];
//						pdu.data[6]=rx_can1_init.data[6];
//						pdu.data[7]=rx_can1_init.data[7];

						memcpy(pdu.data,rx_can1_init.data,8);

   	 			         if((diagnosis_on==1) || (fault_checks_enable==1)){//fault_checks will check continuously after Diagnosis state
   	 			        	if(pdu.data[7]==1){
   	 			            	pdu.fault_alarm=1;
   	 			        	}
   	 			        	else if(pdu.data[7]==0){
   	 			        		pdu.fault_alarm=0;
   	 			        	}
   	 			         }

//   	 			     if(feedback_from_pdu==pdu.data[0]){
//   	 			    	send_data_to_pdu_flag=0;
//
//   	 			     }
   	 			  memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
   	 		 		//	FLEXCAN_DRV_Send(INST_CANCOM1,8U,&can1_data_init,0x1876590,can_data_1);
   	 		 			break;
   	 		 				}

   	 		case from_bms : //BP & BMS  0x1821FFD9 0x18F0E5F6 0x18F139A0
   	 				{
//   	 				     bms.on=1;


//						bms.data[0]=rx_can1_init.data[0];
//						bms.data[1]=rx_can1_init.data[1];
//						bms.data[2]=rx_can1_init.data[2];
//						bms.data[3]=rx_can1_init.data[3];
//						bms.data[4]=rx_can1_init.data[4];
//						bms.data[5]=rx_can1_init.data[5];
//						bms.data[6]=rx_can1_init.data[6];
//						bms.data[7]=rx_can1_init.data[7];
						//  in_charg++;
						memcpy(bms.data,rx_can1_init.data,8);

						bms.voltage=(bms.data[1]*256+bms.data[2]);
						bms.voltage=bms.voltage/100;
						bms.current=(bms.data[3]*256+bms.data[4]);
						bms.current=(bms.current-7000)/10;


						if(bms.data[0]==4)
						{
							send_to_bms=2;///to bms_wake_up;
							sending_data_to_bms();

						}
						else if(bms.data[0]==2)
						{
							send_to_bms=5;
							sending_data_to_bms();//to bms_discharge();
						}
//
//						else if(bms.data[0]==5)
//						{
//							bms.on=1;
////							in_discharging=1;
////							in_charging=0;
//						}
					if((diagnosis_on==1) || (fault_checks_enable==1)){

   						if(bms.data[7]==0){
   							bms.fault_alarm=0;
   						}
   						else if(bms.data[7]>=1){
   							bms.fault_alarm=1;
   						}
   					 }
   	 			  memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
   	 			    // FLEXCAN_DRV_Send(INST_CANCOM1,8U,&can1_data_init, 0x1876590,can_data_1);
   	 			break;
   	 				}
//   	 		case speed_ctrl: //EPS
//   	 		{
//   	 		set_rpm=rx_can1_init.data[0]+rx_can1_init.data[1]*256;
////   	 					eps_on=1;
////   	 				 if((diagnosis_on==1) || (fault_checks_enable==1)){
////   							if(rx_can1_init.data[7]==1){
////   								error_in_eps=0;
////   							}
////   							else if(rx_can1_init.data[7]==0){
////   								error_in_eps=1;
////   							}
////   						 }
//
//   	 				  memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
//   	 		     //     FLEXCAN_DRV_Send(INST_CANCOM1,9U,&can1_data_init, 0x180E4F1,can_data_1);///mail box id should be different to all the can data that are in diff CAN's
//   	 		    break;
//   	 		}
   	 		case from_dc_dc ://DC-DC AUX
   	 		{
   	 				dc_dc_on=1;
   	 			 if((diagnosis_on==1) || (fault_checks_enable==1)){
   						if(rx_can1_init.data[7]==1){
   							error_in_dc_dc=0;
   						}
   						else if(rx_can1_init.data[7]==0){
   							error_in_dc_dc=1;
   						}
   					 }
   	 			//	 FLEXCAN_DRV_Send(INST_CANCOM1,10U,&can1_data_init, 0x180E221,can_data_1);

   	 			break;
   	 		 }
   	 		case from_mcu : //MCU
   	 		{
   	 					mcu.on=1;
   	 				    mcu.data_check=1;
//   	 					mcu.data[0]=rx_can1_init.data[0];
//   	 			    	mcu.data[1]=rx_can1_init.data[1];
//   	 					mcu.data[2]=rx_can1_init.data[2];
//   	 					mcu.data[3]=rx_can1_init.data[3];
//   	 					mcu.data[4]=rx_can1_init.data[4];
//   	 					mcu.data[5]=rx_can1_init.data[5];
//   	 					mcu.data[6]=rx_can1_init.data[6];
//   	 					mcu.data[7]=rx_can1_init.data[7];


   	 				   memcpy(mcu.data,rx_can1_init.data,8);

   	 					//	motor_rpm=motor_can[6]+motor_can[7]*256;
   	 					//motor_rpm=motor_rpm/8;
   	 					//canspeed=(2*3.14*0.825*motor_rpm)/60;
   	 					////canspeed=(3.14*0.630*motor_rpm)/(60*8);//(3.14*wheel dia *moter rpm / gear ratio(8))/60(sec))
   	 					///	canspeed=(3.14*0.630*motor_rpm)/(60*12);//12 is the gear ratio for tata ace
   	 					mcu.voltage=mcu.data[0]+mcu.data[1]*256;
   	 					mcu.voltage=mcu.voltage/10;
   	 					mcu.current=mcu.data[2]+mcu.data[3]*256;
   	 					mcu.current=mcu.current/10;
   	 					mcu.phase_current=mcu.data[4]+mcu.data[5]*256;
   	 					mcu.phase_current=mcu.phase_current/10;
							if(Motor_rpm==false)
							{
								mcu.rpm=mcu.data[6]+mcu.data[7]*256;
								Motor_rpm=true;
							}




						mcu.speed=(3.14*0.620*mcu.rpm)/(60*12);//12 is the gear ratio for tata ace
						//  Input=mcu.rpm;
				 if((diagnosis_on==1) || (fault_checks_enable==1)){
   						if(mcu.voltage>=78){
   							mcu.fault_alarm=0;
   						}
   						else if(mcu.voltage<78){
   							mcu.fault_alarm=1;
   						}
   					 }
   	 			//	FLEXCAN_DRV_Send(INST_CANCOM1,11U,&can1_data_init, 0x180E421,mcu.data);///mail box id should be different to all the can data that are in diff CAN's
   	 				//  rx_can2_init.msgId=0;
   	 		   memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));

   	 		      break;
   	 		}

   	 		case from_imd : //IMD
   	 		{
   	 				imd_on=1;
   	 			 if((diagnosis_on==1) || (fault_checks_enable==1)){
   						if(rx_can1_init.data[7]==1){
   							error_in_imd=0;
   						}
   						else if(rx_can1_init.data[7]==0){
   							error_in_imd=1;
   						}
   				}
   	 			  memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
   	 			 //    FLEXCAN_DRV_Send(INST_CANCOM1,12U,&can1_data_init, 0x180E921,can_data_1);///mail box id should be different to all the can data that are in diff CAN's

   	 			     //	  rx_can2_init.msgId=0;
   	 			 break;
   	 		}
   	 		case from_rfid : //rfid
   	 	   	 		{
						if((rx_can1_init.data[0]==0x11) && (rx_can1_init.data[1]== 0X45)&&(rx_can1_init.data[2] ==0x6E)){
							rfid_tag='B';
						}
						else if((rx_can1_init.data[0]==0x0D) && (rx_can1_init.data[1]== 0X8F)&&(rx_can1_init.data[2] ==0xBD)){
							rfid_tag='A';
						}

						memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
   	 	   	 			break;
   	 	   	 		}
   	 		case from_radar :
					{

						obj_detection=rx_can1_init.data[0];
						if(obj_detection==1){

							PINS_DRV_ClearPins(PTD,1<<3);// caution gpio

							 //resetting All LEDs of obj detection
							PINS_DRV_SetPins(PTD,1<<0);// relay warning

							PINS_DRV_ClearPins(PTC,1<<12);//critical gpio
						}
						else if(obj_detection==2){

							PINS_DRV_ClearPins(PTD,1<<0);// relay warning

							 //resetting LEDs of obj detection

							 PINS_DRV_SetPins(PTD,1<<3);// relay caution
							 PINS_DRV_ClearPins(PTC,1<<12);//critical gpio
						}
						else if(obj_detection==3){

							PINS_DRV_SetPins(PTC,1<<12);// relay critical

							 //resetting All LEDs of obj detection
							PINS_DRV_SetPins(PTD,1<<0);// relay warning
							PINS_DRV_SetPins(PTD,1<<3);// relay caution


						}
						else if(obj_detection==0){
							//resetting All LEDs of obj detection
							PINS_DRV_ClearPins(PTC,1<<12);// relay caution
							PINS_DRV_SetPins(PTD,1<<0);// relay warning
							PINS_DRV_SetPins(PTD,1<<3);// relay caution
						}


						memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
						break;
					}
   	 	 case from_steering :
					{

						steering_check=rx_can1_init.data[0];
						memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
						 break;
					}
   	 	 case door_status_fb :
					{

						door_close=rx_can1_init.data[0];
						memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
						 break;
					}
   	     case from_radar_hb :
   	 					{

   	 						radar.data_check=rx_can1_init.data[1];
   	 						memset(rx_can1_init.data, 0, sizeof(rx_can1_init.data));
   	 						 break;
   	 					}

   	 	}
   	 	}

   	 }

   }

