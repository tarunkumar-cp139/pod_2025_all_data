#include "pin_config.h"
#include "main.h"

bool pin_check=0;
status_t pin_init_check=1;
void pin_config(){
	 while(pin_init_check){
			  	  pin_init_check=PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
			  	  pin_check=0;
	         }
		  pin_check=1;
	//CLOCK_DRV_Init(&clockMan1_InitConfig0);

}
