/*
 * can2.c
 *
 *  Created on: 04-Nov-2025
 *      Author: Tarun
 */

#include"can2.h" //form & to  nos

flexcan_data_info_t  can2_data_init;
 flexcan_msgbuff_t rx_can2_init;

 CanChecks can2 = {
	      .init_check = STATUS_ERROR,
	      .conf_tx_check = STATUS_ERROR,
	      .conf_rx_check = STATUS_ERROR,
	      .init_success = false
	  };

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


    FLEXCAN_DRV_SetRxMaskType(INST_CANCOM1, FLEXCAN_RX_MASK_INDIVIDUAL);
    FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM1,FLEXCAN_MSG_ID_EXT,4U,0x1FFFFFFF);

  	FLEXCAN_DRV_InstallEventCallback(INST_CANCOM2,(flexcan_callback_t)&can2_callback, NULL);
  	FLEXCAN_DRV_Receive(INST_CANCOM2,4U,&rx_can2_init);
  	can2.init_success=1;
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

 		switch(rx_can2_init.msgId){

 		case from_obc :
 			{


			    obc.data[0]=rx_can2_init.data[0];
				obc.data[1]=rx_can2_init.data[1];
				obc.data[2]=rx_can2_init.data[2];
				obc.data[3]=rx_can2_init.data[3];
				obc.data[4]=rx_can2_init.data[4];
				obc.data[5]=rx_can2_init.data[5];
				obc.data[6]=rx_can2_init.data[6];
				obc.data[7]=rx_can2_init.data[7];


				if(obc.data[0]>=65 && obc.data[0]<=90){
		              if(((obc.data[0]==70 )|| (obc.data[0]==82 )||(obc.data[0]==79 )|| (obc.data[0]==67))
									&& ((obc.data[3]==70 )|| (obc.data[3]==82 )||(obc.data[3]==48 )))
							{
								obc.ack=1;
								data_byte=0;



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



							}
		              else if(obc.data[0]=='D')
		              				{
		              					current_state=DIAGNOSTIC;
		              					diagnosis_on=1;
		              				}
		              else if(obc.data[0]== 3)
						{
		            	  current_state=STOP;

						}

				}


 	        // FLEXCAN_DRV_Send(INST_CANCOM2,2U,&can2_data_init, 0x180E4A2,can_data_1);
 	         // FLEXCAN_DRV_Send(INST_CANCOM2,11U,&can2_data_init, 0x222,can_data);
 	        break;
 			}
 	 	}
 	break;
 }
}
}
