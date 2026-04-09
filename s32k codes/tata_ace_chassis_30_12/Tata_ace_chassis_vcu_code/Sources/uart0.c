/*
 * uart0.c
 *
 *  Created on: 29-Oct-2025
 *      Author: Tarun
 */

#include"uart0.h"
bool uart_check=0;
status_t uart_init_check=1;
void uart0_config(){
	while(uart_init_check){

	  		uart_init_check =LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
	  		uart_check=0;
	  	}
	  	uart_check=1;

}
