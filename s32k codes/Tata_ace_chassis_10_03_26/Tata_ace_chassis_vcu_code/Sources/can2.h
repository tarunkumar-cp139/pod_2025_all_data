/*
 * can2.h
 *
 *  Created on: 04-Nov-2025
 *      Author: Tarun
 */

#ifndef CAN2_H_
#define CAN2_H_

#include "main.h"

 extern flexcan_data_info_t  can2_data_init;
  extern flexcan_msgbuff_t rx_can2_init;
  extern status_t can2_init_check,can2_conf_tx_check,can2_conf_rx_check;

  void can2_config(void);
  void can2_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState);
  void sending_data_to_obc(void);
  void sending_data_to_obc_heart_beat(void);
  void sending_ack_to_obc(void);
  void send_trip_status(void);
  extern bool can2_check,trip_status_send;
extern uint8_t can_data_2[8];
extern uint16_t trip_status_tx ,check_sum_trip_ack_rx,check_sum_tx_status;

//extern CanChecks can2;
#endif /* CAN2_H_ */
