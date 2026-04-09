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

extern bool can2_check;
extern uint8_t can_data_2[8];

//extern CanChecks can2;
#endif /* CAN2_H_ */
