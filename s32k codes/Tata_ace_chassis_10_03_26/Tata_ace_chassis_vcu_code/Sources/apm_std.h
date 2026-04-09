/*
 * apm_std.h
 *
 *  Created on: 06-Jan-2026
 *      Author: Tarun
 */

#ifndef APM_STD_H_
#define APM_STD_H_

#include <main.h>
void apm_std_check(int condition_check);
bool stop_check(void);
bool door_status(void);
bool over_speed_check(void);
uint8_t radar_detection_check(void);
void all_ecu_fault_check(void);
void gpio_intrr_config(void);
void gpio_int(void);
bool ready_to_start(void);
extern volatile  uint32_t flag;

extern bool intr_triger, pinstate,pinstate2,temp_brake_release,door_close;


#endif /* APM_STD_H_ */
