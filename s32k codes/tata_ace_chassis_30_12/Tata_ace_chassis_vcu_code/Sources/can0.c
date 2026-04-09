#include "can0.h" ////BRAKE

//#include <string.h>
//#include <stdio.h>
//#include "canCom1.h"
//lexcan_data_info_t can0_data_init;
 // flexcan_msgbuff_t rx_can0_init;
//bool can0_check;
  uint8_t byte_5,byte_6,press_valid,count_inc=0,press_bar=0;
  volatile uint16_t twos16_via_func, twos_comp_u16 ,dec_u16,ones_comp_u16;

 flexcan_data_info_t can0_data_init,can0_data_init_std;
  flexcan_msgbuff_t rx_can0_init,rx_can0_init_std;
 // uint8_t can_data_1[8]={0xA1,0x02,0x03,0x04,0x05,0xB6,0x7A,0x1A};
  CanChecks can0;
  CanChecks can0 = {
      .init_check = STATUS_ERROR,
      .conf_tx_check = STATUS_ERROR,
      .conf_rx_check = STATUS_ERROR,
	  .conf_rx_check_std = STATUS_ERROR,
	  .conf_tx_check_std  = STATUS_ERROR,
      .init_success = false
  };
void can0_config(){
	//can0.init_check=1,can0.conf_tx_check=1,can0.conf_rx_check=1;
	can0_data_init.data_length= 8;
	can0_data_init.is_remote = false;
	can0_data_init.msg_id_type= FLEXCAN_MSG_ID_EXT;

		can0_data_init_std = can0_data_init;
		can0_data_init_std.msg_id_type = FLEXCAN_MSG_ID_STD;

while(can0.init_check || can0.conf_tx_check || can0.conf_rx_check || can0.conf_rx_check_std || can0.conf_tx_check_std){
	can0.init_check = FLEXCAN_DRV_Init(INST_CANCOM3, &canCom3_State, &canCom3_InitConfig0);
	can0.conf_tx_check = FLEXCAN_DRV_ConfigTxMb(INST_CANCOM3,3U,&can0_data_init, 0x180A087);
	can0.conf_tx_check_std= FLEXCAN_DRV_ConfigTxMb(INST_CANCOM3,5U,&can0_data_init_std, 0x180);
	can0.conf_rx_check = FLEXCAN_DRV_ConfigRxMb(INST_CANCOM3,6U,&can0_data_init,0x0);
	can0.conf_rx_check_std = FLEXCAN_DRV_ConfigRxMb(INST_CANCOM3,8U,&can0_data_init_std,from_brake);

	can0.init_success=0;
	if(can0.count>=5){
		break;
	}
	can0.count++;
}

FLEXCAN_DRV_SetRxMaskType(INST_CANCOM3,FLEXCAN_RX_MASK_INDIVIDUAL);

FLEXCAN_DRV_SetRxIndividualMask(INST_CANCOM3,FLEXCAN_MSG_ID_STD, 8U, 0x1FF);// mask



//FLEXCAN_DRV_ConfigTxMb(INST_CANCOM3,4U,&can0_data_init, 0x180A087);
//FLEXCAN_DRV_SetRxMaskType(INST_CANCOM3,FLEXCAN_RX_MASK_GLOBAL);
//FLEXCAN_DRV_SetRxMbGlobalMask(INST_CANCOM3,FLEXCAN_MSG_ID_EXT,0x0);
//FLEXCAN_DRV_SetRxMbGlobalMask(INST_CANCOM3,FLEXCAN_MSG_ID_STD,0x0);
//FLEXCAN_DRV_ConfigRxMb(INST_CANCOM3,2U,&can0_data_init,0x0);
FLEXCAN_DRV_InstallEventCallback(INST_CANCOM3,(flexcan_callback_t)&can0_callback, NULL);
FLEXCAN_DRV_Receive(INST_CANCOM3,6U,&rx_can0_init);
FLEXCAN_DRV_Receive(INST_CANCOM3,8U,&rx_can0_init_std);
can0.init_success=1;


}
void sending_data_to_brake(){

						brake.count=count_inc<<4;///byte 1
						press_valid=16;//byte 2
						byte_5=(press_bar*2)>>4;
						byte_6=((press_bar*2)&0x0f)<<4;

						     dec_u16    = (brake.count+press_valid+byte_5+byte_6);                 // 240
					   ones_comp_u16    = (uint16_t)(~dec_u16);      // one's complement,CONVERT 1's to 0's and vice versa
					   twos_comp_u16    = (uint16_t)(ones_comp_u16 + 1u); // two's complement= one's complement+1 // Expected: ones_comp_u16 = 0xFF0F, twos_comp_u16 = 0xFF10

			            brake.check_sum = twos_comp_u16&0xff;

						brake.tx_data[1]=brake.count;//count
				     	brake.tx_data[0]=brake.check_sum;
						brake.tx_data[2]=0x0;
						brake.tx_data[3]=press_valid;
						brake.tx_data[4]=0x00;
						brake.tx_data[5]=byte_5;
						brake.tx_data[6]=byte_6;
						brake.tx_data[7]=0x0;
						FLEXCAN_DRV_Send(INST_CANCOM3,10U,&can0_data_init_std,to_brake,brake.tx_data);
						count_inc++;
						if(count_inc>=16){
							count_inc=0;///byte 1
						}

}
void can0_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState)
   {
 //(void)buffIdx;
   	(void)flexcanState;

   	(void) instance;

   	switch(eventType){

   	case FLEXCAN_EVENT_RX_COMPLETE:
   	{

   		FLEXCAN_DRV_Receive(INST_CANCOM3,6U,&rx_can0_init);
   		FLEXCAN_DRV_Receive(INST_CANCOM3,8U,&rx_can0_init_std);

   		switch (rx_can0_init_std.msgId){


   		case from_brake ://Brake
   	   	 		{


   	   	 			brake.data[0]=rx_can0_init_std.data[0];
   	   	 			brake.data[1]=rx_can0_init_std.data[1];
   					brake.data[2]=rx_can0_init_std.data[2];
   					brake.data[3]=rx_can0_init_std.data[3];
   					brake.data[4]=rx_can0_init_std.data[4];
   					brake.data[5]=rx_can0_init_std.data[5];
   					brake.data[6]=rx_can0_init_std.data[6];
   					brake.data[7]=rx_can0_init_std.data[7];
   	//   	 		brake.data[0]=brake.data[4]<<8;
   	//   	 		byte5=brake.data[5];

   					applied_press=brake.data[4]<<8|brake.data[5];
   	   	 		  	applied_press=applied_press/10;


   	   	 				if(diagnosis_on==1){
   	   						if((brake.data[2]>>6 )& (0x03) == 1){
   	   							brake.error=1;//fault yellow warning lamp
   	   						}
   	   						else if( (brake.data[2]>>6) &( 0x03 )== 0){
   	   							brake.error=0;
   	   						}
   	   				    	else if((brake.data[2]>>6) & (0x03) == 2){
								brake.error=1;//fault red warning lamp
							}
   	   					 }
   	   	 				//	FLEXCAN_DRV_Send(INST_CANCOM1,12U,&can0_data_init, 0x180E521,can_data_1);///mail box id should be different to all the can data that are in diff CAN's
   	   	 				//	  rx_can2_init.msgId=0;
   	   	 				memset(rx_can0_init_std.data, 0, sizeof(rx_can0_init_std.data));
   	   	 			break;
   	   	 		}
   		}
   	}
   }
 }
//void can0_config(){
//
//		can0_data_init.data_length= 8;
//	  	can0_data_init.is_remote = false;
//	  	can0_data_init.msg_id_type= FLEXCAN_MSG_ID_EXT;
//  	while(can0_init_check && can0_conf_tx_check && can0_conf_rx_check){
//  		can0_init_check = FLEXCAN_DRV_Init(INST_CANCOM3, &canCom3_State, &canCom3_InitConfig0);
//  		can0_conf_tx_check = FLEXCAN_DRV_ConfigTxMb(INST_CANCOM3,4U,&can0_data_init, 0x180A087);
//  		can0_conf_rx_check = FLEXCAN_DRV_ConfigRxMb(INST_CANCOM3,2U,&can0_data_init,0x0);
//  		can0_check=0;
//  	}
//
//
//  	//FLEXCAN_DRV_ConfigTxMb(INST_CANCOM3,4U,&can0_data_init, 0x180A087);
//  	FLEXCAN_DRV_SetRxMaskType(INST_CANCOM3,FLEXCAN_RX_MASK_GLOBAL);
//  	FLEXCAN_DRV_SetRxMbGlobalMask(INST_CANCOM3,FLEXCAN_MSG_ID_EXT,0x0);
//  	//FLEXCAN_DRV_ConfigRxMb(INST_CANCOM3,2U,&can0_data_init,0x0);
//  	FLEXCAN_DRV_InstallEventCallback(INST_CANCOM3,(flexcan_callback_t)&can0_callback, NULL);
//  	FLEXCAN_DRV_Receive(INST_CANCOM3,2U,&rx_can0_init);
//
