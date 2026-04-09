/*
 * system_states.h
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#ifndef SYSTEM_STATES_H_
#define SYSTEM_STATES_H_
#include"main.h"
void system_states(void);
void diagnosis(void);
void sending_data_to_bms(void);
void sending_data_to_obc(void);
void sending_data_to_pdu(void);
#define forward_relay        PINS_DRV_ClearPins(PTE,1<<11)
#define reverse_relay        PINS_DRV_SetPins(PTE,1<<11)
#define throttle_relay_off   PINS_DRV_SetPins(PTA,1<<0)
#define throttle_relay_on    PINS_DRV_ClearPins(PTA,1<<0)
typedef enum {

	INIT,
	WAKE_UP,
	DIAGNOSTIC,
	STOP,
	RUNNING,
	DOOR_OPEN,
	DOOR_CLOSE,
	SLEEP,
	DEEP_SLEEP,
	CHARGE,
	FAULT,
	EMERGENCY

} SystemStates; // System states
extern int count,pid_time_count,obc_hb_count,brake_initial_count;
extern char tx_buff[128];
extern bool obj_detection,brake_initial_check,brake_data_send,steering_check, mcu_data_check,motor_check_on,mcu_start_tmr_check,pid_enable,run_flag,state_status,motor_state,hvac_state,dc_dc_state,charger_contactor_state,send_data_to_pdu_flag;//can1_check=0,
extern uint8_t Critical_System_Status,vcu_id,obc_id,Passenger_load,rpm,obj_dect,Check_Sum,send_to_bms,motor,feedback_from_pdu,data_byte;
extern SystemStates current_state;


#endif /* SYSTEM_STATES_H_ */
