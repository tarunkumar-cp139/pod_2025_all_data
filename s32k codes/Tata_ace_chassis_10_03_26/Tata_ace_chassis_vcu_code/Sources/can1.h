/*
 * can1.h
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#ifndef CAN1_H_
#define CAN1_H_

#include "main.h"
//#include "can0.h"


 extern flexcan_data_info_t can1_data_init,can1_data_init_std;
 extern flexcan_msgbuff_t rx_can1_init,rx_can1_init_std;


  void can1_config(void);
  void can1_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState);
  static void can_config_rx_mb_for_extid_filter(uint8_t mb_idx, uint32_t id);
extern uint16_t applied_press,set_rpm;
extern uint8_t can_data_1[8];
extern char rfid_tag;
extern volatile bool Motor_rpm;
extern uint32_t mask;
typedef struct{
	bool rx_flag,on,error,ack,fault_alarm,can_fault,data_check;
	double rpm;
	double speed;
    float voltage,current,phase_current;
    uint8_t data[8],tx_data[8],check_sum,count_to_send_data,prv_data[8];
    uint16_t rx_check_sum,tx_ack_check_sum,count_fault_check;

}parameter;
extern parameter mcu,bms,brake,pdu,obc,fault,all_ecu,radar;
//extern CanChecks can1;
#endif /* CAN1_H_ */
