#ifndef CAN0_H
#define CAN0_H
#include "main.h"

extern bool data_from_vcu,diagnosis_success,no_error_in_pdu,error_in_pdu,no_error_in_bp,error_in_bp,no_error_in_eps,error_in_eps,no_error_in_mcu,error_in_mcu,no_error_in_brake,error_in_brake,no_error_in_imd,error_in_imd,no_error_in_dc_dc,error_in_dc_dc;
extern bool dc_dc_on,eps_on,brake_on,imd_on,wake_up_success,diagnosis_on;

#define from_pdu   0x1821FFD9
#define to_pdu     0x1821D9A0
#define from_brake 0x10A //:From_Brake
#define to_brake   0x2AC //:TO_Brake
#define from_imd   0x180288F1  //IMD
#define from_mcu   0x180128F1  //MCU
#define from_dc_dc 0x1875FA1 //DC-DC AUX
#define from_bms   0x18F0E5F6  //BP & BMS
#define to_bms     0x18F0E5F5  //BP & BMS
#define from_eps   0x180E2F4 //EPS 0x18A1B2C3
#define to_obc    0x18A1B2C3
#define to_obc_hb  0x18170731
#define from_obc   0x18A1B2C4
#define to_ack_obc   0x18A1B2C5
#define from_rfid  0x18F139A0
#define from_lidar 0x03
#define from_radar 0x186543f3//
#define change_pid 0x18765432//
#define from_steering 0x1A092AB3

extern flexcan_data_info_t can0_data_init,can0_data_init_std;
extern flexcan_msgbuff_t rx_can0_init,rx_can0_init_std;


void can0_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState);
void can0_config(void);
void sending_data_to_brake(void);
extern uint8_t byte_5,byte_6,press_valid,count_inc,press_bar;

typedef struct {
	status_t init_check;
	status_t conf_tx_check,conf_tx_check_std;
	status_t conf_rx_check,conf_rx_check_std;
	bool init_success;
	uint8_t count;
} CanChecks;

extern CanChecks can0,can1,can2,spi0;




#endif
