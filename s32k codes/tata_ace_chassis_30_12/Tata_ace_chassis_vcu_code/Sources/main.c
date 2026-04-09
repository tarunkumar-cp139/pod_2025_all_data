/* ###################################################################
**     Filename    : main.c
**     Processor   : S32K1xx
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including necessary module. Cpu.h contains other modules needed for compiling.*/
#include "main.h"
volatile int exit_code = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */


//uint8_t can_data_1[8]={0xA1,0x02,0x03,0x04,0x05,0xB6,0x7A,0x1A};
/*! 
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
//CLOCK_DRV_Init(&clockMan1_InitConfig0);
    syst_clk();
    pin_config();
    can0_config();//brake
    can1_config();//all ecu
    can2_config();//obc
    uart0_config();
    spi0_config();//dac mcp4921
    timer_config();
    pid_config();
    PINS_DRV_SetPins(PTE,1<<1);///can1 enable
    PINS_DRV_SetPins(PTE,1<<0);//can2 enable

    INT_SYS_SetPriority(LPSPI0_IRQn,0);
	INT_SYS_SetPriority(CAN0_ORed_IRQn,2);
	INT_SYS_SetPriority(CAN1_ORed_IRQn,1);
	INT_SYS_SetPriority(CAN2_ORed_IRQn,3);
	INT_SYS_SetPriority(LPTMR0_IRQn,4);

	PINS_DRV_SetPins(PTD,1<<0);//relay not used
	PINS_DRV_SetPins(PTD,1<<3);//relay not used
while(1){
	system_states();

	if(mcu.rpm>=400 && mcu.rpm<=1000 ){
		current_state =FAULT;//STOP
	}

    OSIF_TimeDelay(1);
}

  /* Write your code here */
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the NXP S32K series of microcontrollers.
**
** ###################################################################
*/
