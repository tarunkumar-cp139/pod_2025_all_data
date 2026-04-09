#include"syst_clk.h"
#include "Cpu.h"

status_t clock_init_check=1;
bool clock_check=0;
void syst_clk(){

	while(clock_init_check){
			  clock_init_check=CLOCK_DRV_Init(&clockMan1_InitConfig0);
			  clock_check=0;
		  }
		  clock_check=1;


}
