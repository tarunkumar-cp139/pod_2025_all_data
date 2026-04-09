/*
 * spi0.h
 *
 *  Created on: 07-Dec-2025
 *      Author: Tarun
 */

#ifndef SPI0_H_
#define SPI0_H_

#include "main.h"

void spi0_config(void);
void MCP4921_SetOutput(uint16_t dac_value);
void pid_config(void);
extern  double consKp, consKi, consKd,Input, Output,Setpoint;
extern PID_TypeDef TPID;
#endif /* SPI0_H_ */
