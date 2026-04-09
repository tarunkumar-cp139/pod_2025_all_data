/*
 * can2.c
 *
 *  Created on: 04-Nov-2025
 *      Author: Tarun
 */

#include"can2.h" //form & to  nos

flexcan_data_info_t  can2_data_init;
 flexcan_msgbuff_t rx_can2_init;
uint16_t trip_status_tx ,check_sum_trip_ack_rx,check_sum_tx_status;
uint32_t dfgh;
uint8_t count_same_data;
bool trip_status_send;
 CanChecks can2 = {
	      .init_check = STATUS_ERROR,
	      .conf_tx_check = STATUS_ERROR,
	      .conf_rx_check = STATUS_ERROR,
	      .init_success = false
	  };
 static void can_config_rx_mb_for_extid_filter(uint8_t mb_idx, uint32_t id)
 {

     // 1) Configure MB with the specific ID
    // FLEXCAN_DRV_ConfigRxMb(CAN_INSTANCE, mb_idx, &rxInfo, id);
     FLEXCAN_DRV_ConfigRxMb(INST_CANCOM2,mb_idx,&can2_data_init, id);
     // 2) Exact-match mask: 29 ID bits + IDE
     uint32_t mask = 0x1FFFFFFF;    // all 29 ID bits must match
    // mask |= (1UL << 30);           // IDE bit must be 1 (extended frame)
     // mask |= (1UL << 31);        // optional: RTR must match too

     FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM2,FLEXCAN_MSG_ID_EXT , mb_idx,mask);
     FLEXCAN_DRV_Receive(INST_CANCOM2, mb_idx,&rx_can2_init);
 }

void can2_config(){
	       // can2.init_check=1,can2.conf_tx_check=1,can2.conf_rx_check=1;
	        can2_data_init.data_length= 8;
	    	can2_data_init.is_remote = false;
	    	can2_data_init.msg_id_type= FLEXCAN_MSG_ID_EXT;
    while(can2.init_check || can2.conf_tx_check || can2.conf_rx_check){
  		can2.init_check = FLEXCAN_DRV_Init(INST_CANCOM2, &canCom2_State, &canCom2_InitConfig0);
  		can2.conf_tx_check = FLEXCAN_DRV_ConfigTxMb(INST_CANCOM2,3U,&can2_data_init, 0x180e5F4);
  		can2.conf_rx_check = FLEXCAN_DRV_ConfigRxMb(INST_CANCOM2,4U,&can2_data_init, from_obc);
  		can2.init_success=0;
  		if(can2.count>=5){
  				break;
  			}
  			can2.count++;
    }
    FLEXCAN_DRV_SetRxMaskType(INST_CANCOM2,FLEXCAN_RX_MASK_INDIVIDUAL);
      	FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM2,FLEXCAN_MSG_ID_EXT,4U,mask);

    can_config_rx_mb_for_extid_filter(5U, from_obc_hb );
    can_config_rx_mb_for_extid_filter(6U, trip_status_ack_rx_id);
    can_config_rx_mb_for_extid_filter(7U, from_gps);

  	//FLEXCAN_DRV_SetRxMbGlobalMask(INST_CANCOM2,FLEXCAN_MSG_ID_EXT,0x0);
  	FLEXCAN_DRV_InstallEventCallback(INST_CANCOM2,(flexcan_callback_t)&can2_callback, NULL);
  	FLEXCAN_DRV_Receive(INST_CANCOM2,4U,&rx_can2_init);
  	FLEXCAN_DRV_Receive(INST_CANCOM2,5U,&rx_can2_init);
  	FLEXCAN_DRV_Receive(INST_CANCOM2,6U,&rx_can2_init);
	FLEXCAN_DRV_Receive(INST_CANCOM2,7U,&rx_can2_init);
  	can2.init_success=1;
  }
void sending_data_to_obc(){
							obc.tx_data[0]=current_state;
							obc.tx_data[1]=state_status;
							obc.tx_data[2]=Critical_System_Status;
							obc.tx_data[3]=rfid_tag;
							obc.tx_data[4]=obj_detection;
							obc.tx_data[5]=(uint8_t)mcu.speed;//Pay_load;
							obc.tx_data[6]=door_status();//rpm
							obc.tx_data[7]=obc.check_sum;

    FLEXCAN_DRV_Send(INST_CANCOM2,14U,&can2_data_init,to_obc,obc.tx_data);///to obc

}
void send_trip_status(){
					check_sum_tx_status=0;

					trip_data_buf[0]=trip_status;
					trip_data_buf[1]=door_close;
					trip_data_buf[2]=0;
					trip_data_buf[3]=0;
					trip_data_buf[4]=0;
					trip_data_buf[5]=0;
					for(uint8_t i=0 ;i<6;i++){
						check_sum_tx_status=trip_data_buf[i]+check_sum_tx_status;
					}

					trip_data_buf[6]=check_sum_tx_status & 0xff;
					trip_data_buf[7]=check_sum_tx_status>>8 & 0xff;

		FLEXCAN_DRV_Send(INST_CANCOM2,13U,&can2_data_init,trip_status_tx_id,trip_data_buf);///to obc

}
void sending_ack_to_obc(){
							obc.tx_data[0]= (uint8_t)(from_obc>>0 & 0xff);
							obc.tx_data[1]= (uint8_t)(from_obc>>8 & 0xff);
							obc.tx_data[2]=(uint8_t)(from_obc>>16 & 0xff);
							obc.tx_data[3]=(uint8_t)(from_obc>>24 & 0xff);
							obc.tx_data[4]=(uint8_t)(obc.rx_check_sum & 0xff);//lsb of rx check sum
							obc.tx_data[5]=(uint8_t)(obc.rx_check_sum>>8 & 0xff);//msb of rx check sum
							obc.tx_ack_check_sum=obc.tx_data[0]+obc.tx_data[1]+obc.tx_data[2]+obc.tx_data[3]+obc.tx_data[4]+obc.tx_data[5];
							obc.tx_data[6]=(uint8_t)(obc.tx_ack_check_sum & 0xff);
							obc.tx_data[7]=(uint8_t)(obc.tx_ack_check_sum>>8 & 0xff);

    FLEXCAN_DRV_Send(INST_CANCOM2,12U,&can2_data_init,to_ack_obc,obc.tx_data);///to obc

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

}

void can2_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState)
 {

 	(void)flexcanState;

 	(void) instance;

 	switch(eventType){

 	case FLEXCAN_EVENT_RX_COMPLETE:
 	{
 ///assign mailbox execpt 2&4

 		FLEXCAN_DRV_Receive(INST_CANCOM2,4U,&rx_can2_init);
 		FLEXCAN_DRV_Receive(INST_CANCOM2,5U,&rx_can2_init);
 		FLEXCAN_DRV_Receive(INST_CANCOM2,6U,&rx_can2_init);
 		FLEXCAN_DRV_Receive(INST_CANCOM2,7U,&rx_can2_init);
 		switch(rx_can2_init.msgId){

 		case from_obc :
 			{


				memcpy(obc.data,rx_can2_init.data,8);//to copy the rx can data (rx_can2_init.data) to  obc.data

				if(obc.data[0]>=65 && obc.data[0]<=90)
				{
		              if(((obc.data[0]==70 )|| (obc.data[0]==82 )||(obc.data[0]==79 )|| (obc.data[0]==67))
									&& ((obc.data[3]==70 )|| (obc.data[3]==82 )||(obc.data[3]==48 )))
							{
		            	        obc.rx_check_sum=0;
								obc.ack=1;
								data_byte=0;
								trip_status_send=1;
									while(data_byte<6){
										obc.rx_check_sum=obc.data[data_byte]+obc.rx_check_sum;
										if(obc.prv_data[data_byte]==obc.data[data_byte])
											  count_same_data++;
										data_byte++;
										//obc.prv_data[data_byte]=obc.data[data_byte];
									}
								data_byte=0;
								if(count_same_data<=3){
									if(obc.rx_check_sum == (obc.data[6]+obc.data[7]*256)){
											if(obc.data[data_byte]=='F' || obc.data[data_byte]=='R')
											{
												run_flag=1;
												current_state=RUNNING;
											}
											else if(obc.data[data_byte]=='O')
											{
												current_state=DOOR_OPEN;
											}
											else if(obc.data[data_byte]=='C')
											{
												current_state=DOOR_CLOSE;
											}
											//i=0;
											data_byte=0;
											sending_ack_to_obc();
									}
									else {
										data_byte=0;
										sending_ack_to_obc();
									}

							  }
							  else{
								  /////if the same data received multiple times then fault will trigger

							  }

				         }
						else if(obc.data[0]=='D')
								{
									state_status=0;
									current_state=DIAGNOSTIC;
									diagnosis_on=1;
								}

			}
			else if(obc.data[0]=='3')
			{
				current_state=STOP;

			}
				memcpy(	obc.prv_data,obc.data,6);
				memset(rx_can2_init.data, 0, sizeof(rx_can2_init.data));
				count_same_data=0;
 	        // FLEXCAN_DRV_Send(INST_CANCOM2,2U,&can2_data_init, 0x180E4A2,can_data_1);
 	         // FLEXCAN_DRV_Send(INST_CANCOM2,11U,&can2_data_init, 0x222,can_data);
 	        break;
 			}
 		case from_obc_hb :
			{

				hb_check_obc=1;
				memset(rx_can2_init.data, 0, sizeof(rx_can2_init.data));
				break;
			}

 		case trip_status_ack_rx_id :
 					{
 						check_sum_trip_ack_rx=0;
 						    memcpy(trip_ack_rx_buf,rx_can2_init.data,8);
//

						//	dfgh=((trip_ack_rx_buf[0]<<0 )|(trip_ack_rx_buf[1]<<8 )|(trip_ack_rx_buf[2]<<16)|(trip_ack_rx_buf[3]<<24));
						//	dfgh=((trip_ack_rx_buf[4]<<0 )|(trip_ack_rx_buf[5]<<8));
//							sprintf(tx_buff,"Deep Sleep\r\n");
//																             	LPUART_DRV_SendData(INST_LPUART1,tx_buff,strlen(tx_buff));

							for(uint8_t i=0 ;i<6;i++){
							check_sum_trip_ack_rx =trip_ack_rx_buf[i]+check_sum_trip_ack_rx;
							}


							if((uint32_t)(trip_status_tx_id )== ((trip_ack_rx_buf[0]<<0 )|(trip_ack_rx_buf[1]<<8 )|(trip_ack_rx_buf[2]<<16)|(trip_ack_rx_buf[3]<<24)))
							{

								if(check_sum_tx_status==((trip_ack_rx_buf[4]<<0 )|(trip_ack_rx_buf[5]<<8))){
									   trip_status_send=0;

								}
							}

							memset(trip_ack_rx_buf, 0, sizeof(trip_ack_rx_buf));
							memset(rx_can2_init.data, 0, sizeof(rx_can2_init.data));

 						break;
 					}
 		case from_gps : //gps
				{
					if((rx_can2_init.data[0]==0x42))// && (rx_can1_init.data[1]== 0X45)&&(rx_can1_init.data[2] ==0x6E)){
					{
							rfid_tag='B';
					}
					else if((rx_can2_init.data[0]==0x41)) //&& (rx_can1_init.data[1]== 0X8F)&&(rx_can1_init.data[2] ==0xBD)){
					{
						rfid_tag='A';
					}

					memset(rx_can2_init.data, 0, sizeof(rx_can2_init.data));
					break;
				}
 	 	}
 	break;
  }
 }
}
