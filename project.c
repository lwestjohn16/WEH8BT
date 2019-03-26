#include <stdint.h>
#include "stm32f4xx.h"
#include "queue.h"
#include "initilization.h"
#include "tasks.h"

int main(){
	init_spi();
	init_display();
	while(1){
		bluetooth();
		sound();
		display();
		//clear_display();
	}
}
