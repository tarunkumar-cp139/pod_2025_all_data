/*
 * uart0.h
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#ifndef UART0_H_
#define UART0_H_
#include "main.h"
void uart0_config(void);
extern char tx_buff[128];

extern bool uart_check , pin_check;
extern status_t uart_init_check,pin_init_check;
#endif /* UART0_H_ */
