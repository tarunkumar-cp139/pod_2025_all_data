#ifndef CAN0_H
#define CAN0_H
#include "main.h"

extern bool data_from_vcu,diagnosis_success,no_error_in_pdu,error_in_pdu,no_error_in_bp,error_in_bp,no_error_in_eps,error_in_eps,no_error_in_mcu,error_in_mcu,no_error_in_brake,error_in_brake,no_error_in_imd,error_in_imd,no_error_in_dc_dc,error_in_dc_dc;
extern bool dc_dc_on,eps_on,brake_on,imd_on,wake_up_success,diagnosis_on;




#define from_lidar		        0x03

#define from_brake 				0x10A //:From_Brake

#define to_brake   				0x2AC //:TO_Brake

#define to_obc_hb              0x180ABC09 //0x18170731

#define from_obc               0x180ABC02// 0x18A1B2C4

#define from_obc_hb            0x180ABC04// 0x18170732

#define to_ack_obc             0x180ABC05// 0x18A1B2C5

#define to_obc                 0x180ABC03// 0x18A1B2C3

#define trip_status_tx_id	   0x180ABC07	//0x18101212

#define trip_status_ack_rx_id   0x180ABC06 //0x18101213//  trip_status_acknowledge

#define from_pdu   				0x1821FFD9

#define to_pdu     				0x1821D9A0

#define from_bms	        	0x18F0E5F6

#define to_bms					0x18F0E5F5

#define from_mcu				0x180128F1

#define from_imd   				0x180288F1  //IMD

#define from_rfid  			    0x180ACA04 //0x18F139A0

#define from_gps 				0x180ACA03 //0x18F139A1

#define from_radar			    0x180ACA01 ///0x186543f3//

#define from_steering			0x18FACA02 //0x1A092AB3

#define door_status_fb          0x180ACC03 // 0x18A123A1

#define to_all_ecu_hb           0x18FACC01 // 0x18170721

#define speed_ctrl				0x180E2F4

#define from_dc_dc			    0x1875FA1

#define from_radar_hb	 		0x18FACC02

#define VCU_fault	 			0x18000123


//from_rfid	0x18F139A0
//from_gps	0x18F139A1
//from_radar	0x186543f3
//from_steering	0x1A092AB3
//door_status_fb	0x18A123A1


//
//from_lidar	0x03	0x03
//from_brake	0x10A	0x10A
//to_brake	0x2AC 	0x2AC
//from_pdu	0x1821FFD9	0x1821FFD9
//to_pdu	0x1821D9A0	0x1821D9A0
//from_imd	0x180288F1	0x180288F1
//from_mcu	0x180128F1	0x180128F1
//from_dc_dc	0x1875FA1 	0x1875FA1
//from_bms	0x18F0E5F6	0x18F0E5F6
// to_bms	0x18F0E5F5	0x18F0E5F5
//from_eps	0x180E2F4 	0x180E2F4
//from_obc 	0x18A1B2C4	0x18FABC02
//from_obc_hb	 0x18170732	 0x18FABC04
//to_ack_obc	 0x18A1B2C5	 0x18FABC05
//to_obc	0x18A1B2C3	0x18FABC03
//trip_status_tx_id	0x18101212	0x18FABC07
//trip_status_ack_rx_id	0x18101213	0x18FABC06
//from_rfid	0x18F139A0	0x18FACC02
//from_gps	0x18F139A1	0x18FACC01
//from_radar	0x186543f3	0x18FACA01
//from_steering	0x1A092AB3	0x18FACA02
//door_status_fb	0x18A123A1	0x18FACC03

extern flexcan_data_info_t can0_data_init,can0_data_init_std;
extern flexcan_msgbuff_t rx_can0_init,rx_can0_init_std;


void can0_callback (uint8_t instance, flexcan_event_type_t eventType,uint32_t buffIdx, flexcan_state_t flexcanState);
void can0_config(void);
void sending_data_to_brake(void);
bool brake_released(void);
extern uint8_t byte_5,byte_6,press_valid,count_inc,press_bar,press_applied;

typedef struct {
	status_t init_check;
	status_t conf_tx_check,conf_tx_check_std;
	status_t conf_rx_check,conf_rx_check_std;
	bool init_success;
	uint8_t count;
} CanChecks;

extern CanChecks can0,can1,can2,spi0;




#endif
