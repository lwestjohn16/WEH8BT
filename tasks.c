#include "queue.h"
#include "stm32f4xx.h"
#include "inttypes.h"
#include "tasks.h"

queue_t q1 = {0,0,20};
queue_t q2 = {0,0,20};

void bluetooth(void){
	/*static int counter = 0;
	if(counter == 0){SPI3->CR1 &=~ SPI_CR1_SSI;}
	if(counter == 1){SPI3->CR1 |= SPI_CR1_SSI;}*/
		int16_t data = 0; //data from Bluetooth packet
		int16_t sound = 0; // data to be put into sound queue
		if (SPI_SR_RXNE & SPI3->SR) {
				// From experience -- the flag is a little early, so wait a bit ...
				data = SPI3->DR;
				SPI3->DR = 0;
				data = (data << 12);
				data = (data >> 12);
				if(data > 0){
					if((data < 6) || (data > 0))
						sound = 1;
					else
						sound = 0;
					put_q(&q1, &data);
					put_q(&q2, &sound);
				}
			//counter++;
		}
		//if(counter == 3)
			//counter = 0;	
}

void check_spi(){
	while (SPI1->SR & SPI_SR_BSY) {	 
		for (int32_t i = 0; i < 1000; i++){	
			GPIOA->ODR |= GPIO_ODR_OD4; 
		}
	}
	for (int32_t i = 0; i<100; i++); 
}
	
void clear_display(){
	/*for(int i=0; i<150; i++){ 				// cycle through column address
		GPIOA->ODR &=~ GPIO_ODR_OD0; 		// set n_cd low
		int c1; 							// column 1 command
		int c2; 							// column 2 command
		int c_msb; 
		int c_lsb;
		c_msb = (i & 0xF0)>>4;
		c_lsb = i & 0x0F;
		c1 = c_lsb;
		c2 = 0x10 | c_msb;
		SPI1->DR = c1; 						// send 1st column command
		check_spi();							
		SPI1->DR = c2; 						// send 2nd column command
		check_spi();		
		for(int b=0; b<9 ; b++){ 			// cycle through page address
			int pagenum = 0xB0 | b;
			GPIOA->ODR &=~ GPIO_ODR_OD0; 	// set n_cd low
			SPI1->DR = pagenum; 			// send page command
			check_spi();	
			GPIOA->ODR |= GPIO_ODR_OD0; 	// set n_cd high
			SPI1->DR = 0x00;
			check_spi();		
		}
	}*/
	GPIOA->ODR &= ~GPIO_ODR_OD0;
    for(int i = 0; i < 8; i++){
			GPIOA->ODR &= ~GPIO_ODR_OD0;
			for (int32_t k = 0; k < 1500; k++){
			}
			if (~SPI1->SR & SPI_SR_BSY) {
				// From experience -- the flag is a little early, so wait a bit ...
				// Then pull PA0 low, and transmit
				SPI1->DR = (0xb0 | i);
			}
			for(int j = 0; j < 102; j++){
					for (int32_t k = 0; k < 1500; k++){
				  }
					GPIOA->ODR &= ~GPIO_ODR_OD0;
					if (~SPI1->SR & SPI_SR_BSY) {
						// From experience -- the flag is a little early, so wait a bit ...
						//transmit least significant bit of column
						SPI1->DR = (j & 0x0f);
					}
					for (int32_t k = 0; k < 1500; k++){
				  }
					GPIOA->ODR &= ~GPIO_ODR_OD0;
					if (~SPI1->SR & SPI_SR_BSY) {
						// From experience -- the flag is a little early, so wait a bit ...
						//transmit least significant bit of column
						SPI1->DR = ((j >> 4) | 0x10);
					}
					for (int32_t k = 0; k < 1500; k++){
				  }
					if (~SPI1->SR & SPI_SR_BSY) {
						// From experience -- the flag is a little early, so wait a bit ...
						//transmit least significant bit of column
						GPIOA->ODR |= GPIO_ODR_OD0;
						SPI1->DR = 0x00;
					}
					for (int32_t k = 0; k < 1500; k++){
				  }
					GPIOA->ODR &= ~GPIO_ODR_OD0;
			}
		}
}
	
void set_page(uint8_t page){
	int pagenum = 0xB0 | page;
	GPIOA->ODR &=~ GPIO_ODR_OD0; 			// set n_cd low
	SPI1->DR = pagenum; 					// send page command
	for (int32_t i = 0; i<1000; i++);		// wait for SPI (CHANGE LATER)	
}
	
void set_column(uint8_t column){
	GPIOA->ODR &=~ GPIO_ODR_OD0; 			// set n_cd low
	int c1; 								// column 1 command
	int c2; 								// column 2 command
	int c_msb; 
	int c_lsb;
	c_msb = (column & 0xF0)>>4;
	c_lsb = column & 0x0F;
	c1 = c_lsb;
	c2 = 0x10 | c_msb;
	SPI1->DR = c1; 							// send 1st column command
	for (int32_t i = 0; i<1000; i++); 		// wait for SPI (CHANGE LATER)						
	SPI1->DR = c2; 							// send 2nd column command
	for (int32_t i = 0; i<1000; i++);		// wait for SPI (CHANGE LATER)
}

void send_data(uint8_t byte){
	uint8_t b; 
	if(byte==0|byte==8|byte==16|byte==24|byte==32|byte==40|byte==48|byte==56) {b = 0x01;}
	if(byte==1|byte==9|byte==17|byte==25|byte==33|byte==41|byte==49|byte==57) {b = 0x02;}
	if(byte==2|byte==10|byte==18|byte==26|byte==34|byte==42|byte==50|byte==58){b = 0x04;}
	if(byte==3|byte==11|byte==19|byte==27|byte==35|byte==43|byte==51|byte==59){b = 0x08;}
	if(byte==4|byte==12|byte==20|byte==28|byte==36|byte==44|byte==52|byte==60){b = 0x10;}
	if(byte==5|byte==13|byte==21|byte==29|byte==37|byte==45|byte==53|byte==61){b = 0x20;}
	if(byte==6|byte==14|byte==22|byte==30|byte==38|byte==46|byte==54|byte==62){b = 0x40;}
	if(byte==7|byte==15|byte==23|byte==31|byte==39|byte==47|byte==55|byte==63){b = 0x80;}
	if(byte == 64){b = 0xFF;}
	for (int32_t i = 0; i<1000; i++);		// wait for SPI (CHANGE LATER)	
	GPIOA->ODR |= GPIO_ODR_OD0; 			// set n_cd high
	SPI1->DR = b;
	for (int32_t i = 0; i<1000; i++);		// wait for SPI (CHANGE LATER)	
}	


void paintpixel(uint16_t x,uint16_t y){
	int page;
	int col;
	int data;
	page = y/8;
	data = y;
	col =x;
	set_page(page);
	set_column(col);
	send_data(data);
}

void circle(){
	for(uint16_t v=36;v<66;v++){paintpixel(v,5);}
	for(uint16_t v=36;v<66;v++){paintpixel(v,50);}
	uint16_t z = 6;
	for(uint16_t x=66;x<76;x++){ paintpixel(x,z);z++;}
	z=6;
	for(uint16_t x=36;x>25;x--){ paintpixel(x,z);z++;}
	z=40;
	for(uint16_t x=26;x<36;x++){ paintpixel(x,z);z++;}
	z=40;
	for(uint16_t x=76;x>65;x--){ paintpixel(x,z);z++;}
	for(uint16_t p=2;p<5;p++){
		set_page(p);
		set_column(26);
		send_data(64);
		set_page(p);
		set_column(76);
		send_data(64);
	}
}


void square(){
	for(uint16_t v=26;v<76;v++){paintpixel(v,7);}
	for(uint16_t v=26;v<76;v++){paintpixel(v,47);}
	for(uint16_t p=1;p<6;p++){
		set_page(p);
		set_column(26);
		send_data(64);
		set_page(p);
		set_column(76);
		send_data(64);
	}
}


void triangle(){
	for(uint16_t v=5;v<95;v++){paintpixel(v,56);}
	uint16_t z = 55;
	for(uint16_t x=5;x<50;x++){ paintpixel(x,z);z--;}
	z=55;
	for(uint16_t x=95;x>50;x--){ paintpixel(x,z);z--;}
}


void smile(){
	for(uint16_t v=36;v<66;v++){paintpixel(v,5);}	//paint circle code (lines 159 - 176)
	for(uint16_t v=36;v<66;v++){paintpixel(v,50);}
	uint16_t z = 6;
	for(uint16_t x=66;x<76;x++){ paintpixel(x,z);z++;}
	z=6;
	for(uint16_t x=36;x>25;x--){ paintpixel(x,z);z++;}
	z=40;
	for(uint16_t x=26;x<36;x++){ paintpixel(x,z);z++;}
	z=40;
	for(uint16_t x=76;x>65;x--){ paintpixel(x,z);z++;}
	for(uint16_t p=2;p<5;p++){
		set_page(p);
		set_column(26);
		send_data(64);
		set_page(p);
		set_column(76);
		send_data(64);
	}																										
	for(uint16_t v=36;v<66;v++){paintpixel(v,35);}	//paint smile code (lines 177 -191)
	z=35;	
	paintpixel(70,31);
	paintpixel(69,32);
	paintpixel(68,33);
	paintpixel(67,34);
	paintpixel(66,35);
	paintpixel(36,35);										
	paintpixel(35,34);
	paintpixel(34,33);
	paintpixel(33,32);
	paintpixel(32,31);
	paintpixel(36,15);
	paintpixel(66,15);
}

void checkmark(){
	uint16_t z = 10;
	for(uint16_t x=10;x<45;x++){ paintpixel(x,z);z++;}
	for(uint16_t x=46;x<55;x++){ paintpixel(x,z);z--;}
}

void error(){
	for(uint16_t v=36;v<66;v++){paintpixel(v,5);}
	for(uint16_t v=36;v<66;v++){paintpixel(v,50);}
	uint16_t z = 6;
	for(uint16_t x=66;x<76;x++){ paintpixel(x,z);z++;}
	z=6;
	for(uint16_t x=36;x>25;x--){ paintpixel(x,z);z++;}
	z=40;
	for(uint16_t x=26;x<36;x++){ paintpixel(x,z);z++;}
	z=40;
	for(uint16_t x=76;x>65;x--){ paintpixel(x,z);z++;}
	for(uint16_t p=2;p<5;p++){
		set_page(p);
		set_column(26);
		send_data(64);
		set_page(p);
		set_column(76);
		send_data(64);
	}
	for(uint16_t v=36;v<66;v++){paintpixel(v,35);}
	z=35;
	paintpixel(66,35);
	paintpixel(67,36);
	paintpixel(68,37);
	paintpixel(69,38);
	paintpixel(70,39);
	paintpixel(32,39);
	paintpixel(33,38);
	paintpixel(34,37);
	paintpixel(35,36);
	paintpixel(36,35);
	paintpixel(36,15);
	paintpixel(66,15);
}	
	

void display(void){
	static int16_t IDNum;
	//clear_display();
  if(~get_q(&q1, &IDNum)){
			if(IDNum != 0){
				clear_display();
				// replace # with get from queue command
				if(IDNum == 1){circle(); }		// paint circle
				else if(IDNum == 2){square();} 	// paint square
				else if(IDNum == 3){triangle();} // paint triangle
				else if(IDNum == 4){smile();} 	// paint smile
				else if(IDNum == 5){checkmark();} 	// paint checkmark
				else{error();}		// paint frown/error message	
		}	
	}
	IDNum = 0;
}

	/* Stripes display and clear code
	int color = 0;
	for(int i=0; i<132; i++){ 	 // cycle through column address
	GPIOA->ODR &=~ GPIO_ODR_OD0;  // set n_cd low
	int c1; 					// column 1 command
	int c2;  					// column 2 command
	int c_msb; 
	int c_lsb;
	c_msb = (i & 0xF0)>>4;
	c_lsb = i & 0x0F;
	c1 = c_lsb;
	c2 = 0x10 | c_msb;
	SPI1->DR = c1; 				// send 1st column command
	check_spi();				
	SPI1->DR = c2; 				// send 2nd column command
	check_spi();	
	for(int b=0; b<8 ; b++){ 			 // cycle through page address
		int pagenum = 0xB0 | b;
		GPIOA->ODR &=~ GPIO_ODR_OD0; // set n_cd low
		SPI1->DR = pagenum; 		 // send page command
	check_spi();
	//for (int32_t i = 0; i<1000; i++);		 // wait for SPI (CHANGE LATER)	
	GPIOA->ODR |= GPIO_ODR_OD0; 				// set n_cd high
	if(color == 1){SPI1->DR = 0x00 ; color = 0;}
	else{SPI1->DR = 0xFF; color = 1;}			// send data to display
	check_spi();
	//for (int32_t i = 0; i<1000; i++);			// wait for SPI (CHANGE LATER)	
	}
	//while(1==0){;}
//}
					
//clear_display();

//paintpixel(0,20); */

void check_spi2(){
							while (SPI2->SR & SPI_SR_BSY) {	 
									for (int32_t i = 0; i < 1000; i++){;}
							}
							for (int32_t i = 0; i<100; i++); 
}

void sound(void){
			int16_t Tone_bytes;
			uint16_t C_index = 0;
		uint16_t E_index = 0;
		uint16_t G_index = 0;
 	  if(~get_q(&q2, &Tone_bytes)){																															// replace "1" with get from queue function
	
			uint16_t note_C5[30] = {250,0,250,0,250,0,250,0,250,0,250,0,250,0,250,
													 0,250,0,250,0,250,0,250,0,250,0,250,0,250,0};
		
			uint16_t note_E5[24] = {500, 0, 500, 0, 500, 0, 500, 0, 500, 0, 500, 0,
													 500, 0, 500, 0, 500, 0, 500, 0, 500 ,0, 500, 0};
		
			uint16_t note_G5[24] = {1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000,
													 0,1000, 0, 1000, 0, 1000, 0, 1000, 0, 1000 ,0, 1000, 0};
			
			GPIOC->ODR &= ~GPIO_ODR_OD0; 																													//set CS low	
		
		
			if(Tone_bytes == 1){
				for (int32_t j = 0; j < 5; j++){																										//send sound 1
					for (int32_t i = 0; i < 31; i++){
						uint16_t value = 0;	
						value += note_C5[C_index];
						value = 0x7000 + (value << 2);	    
						GPIOC->ODR &= ~GPIO_ODR_OD0; 																										//turn off CS bit
						SPI2->DR = (uint8_t)(value>>8);																									//send spi byte  one
						check_spi2();																																		// for loop wait
						SPI2->DR = (uint8_t) value;																											// send spi bite 2
						check_spi2();																																		// for loop wait 
						GPIOC->ODR |= GPIO_ODR_OD0;																											// turn on Cs bit
						for (int32_t i = 0; i < 1000; i++){;}																						// for loop wait
						GPIOC->ODR &= ~GPIO_ODR_OD1;																										// clear Ldac bit
						check_spi2();
						GPIOC->ODR |= GPIO_ODR_OD1;																											// set LDAC bit
						C_index++;
						if(C_index == 30){C_index =0;}
				}
			}
				for (int32_t j = 0; j < 5; j++){																										// send sound 2
					for (int32_t i = 0; i < 31; i++){
						uint16_t value = 0;	
						value += note_E5[E_index];
						value = 0x7000 + (value << 2);	    
						GPIOC->ODR &= ~GPIO_ODR_OD0; 																										//turn off CS bit
						SPI2->DR = (uint8_t)(value>>8);																									//send spi byte  one
						check_spi2();																																		// for loop wait
						SPI2->DR = (uint8_t) value;																											// send spi bite 2
						check_spi2();																																		// for loop wait 
						GPIOC->ODR |= GPIO_ODR_OD0;																											// turn on Cs bit
						for (int32_t i = 0; i < 1000; i++){;}																						// for loop wait
						GPIOC->ODR &= ~GPIO_ODR_OD1;																										// clear Ldac bit
						check_spi2();
						GPIOC->ODR |= GPIO_ODR_OD1;																											// set LDAC bit
						E_index++;
						if(E_index == 24){E_index =0;}
				}
			} 
				for (int32_t j = 0; j < 5; j++){																										//send sound 3
					for (int32_t i = 0; i < 31; i++){
						uint16_t value = 0;	
						value += note_G5[G_index];
						value = 0x7000 + (value << 2);	    
						GPIOC->ODR &= ~GPIO_ODR_OD0; 																										//turn off CS bit
						SPI2->DR = (uint8_t)(value>>8);																									//send spi byte  one
						check_spi2();																																		// for loop wait
						SPI2->DR = (uint8_t) value;																											// send spi bite 2
						check_spi2();																																		// for loop wait 
						GPIOC->ODR |= GPIO_ODR_OD0;																											// turn on Cs bit
						for (int32_t i = 0; i < 1000; i++){;}																						// for loop wait
						GPIOC->ODR &= ~GPIO_ODR_OD1;																										// clear Ldac bit
						check_spi2();
						GPIOC->ODR |= GPIO_ODR_OD1;																											// set LDAC bit
						G_index++;
						if(G_index == 24){G_index =0;}
				}
			}
		}
	}
		Tone_bytes = 0;
}
