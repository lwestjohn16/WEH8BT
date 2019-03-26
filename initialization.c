#include "stm32f4xx.h"
#include "initilization.h"
#include <stdint.h>

void init_display(void){
	// Enable GPIO-A control (required by SCK = PA_5, MOSI = PA_7)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	
	//RCC->CFGR |= RCC_CFGR_PPRE2_2;
	// A pin can be driven by the ODR or some peripheral. 
	// We choose one of the peripherals, the SPI. This is called an "Alt Fnc."
	//FIRST - we'll set the I/O driver to listen to the peripherals instead
	// of listening to the Output Data Register. This is called the "mode",
	// and it is like the DDR of an 8-bit micro-controller.
	// Actually, the STM32f offers 4 options, not just in/out:
	// (00) Tristate/input to IDR; (01) data output asserted from ODR
	// (10) Alternate fnc mode from peripheral; (11) Analog input.
	// Tell the GPIO multiplexer to connect PA5 and 7 to their alternate fnc.
	// Reference Guide, page 157
	GPIOA->MODER &=~ (GPIO_MODER_MODER5_0);
	GPIOA->MODER |=	(GPIO_MODER_MODER5_1); 

	GPIOA->MODER &=~ (GPIO_MODER_MODER7_0);
	GPIOA->MODER |=	(GPIO_MODER_MODER7_1); 
		
	GPIOA->MODER &=~ (GPIO_MODER_MODER0_1 | GPIO_MODER_MODER0_0); 		// nC_D output == CD output
	GPIOA->MODER |=	(GPIO_MODER_MODER0_0);
	
	GPIOA->MODER &=~ (GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); 		// nCS0 output == CS output
	GPIOA->MODER |=	(GPIO_MODER_MODER1_0);
	
	GPIOA->MODER &=~ (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER6_0); 		// nReset output == Reset
	GPIOA->MODER |=	(GPIO_MODER_MODER6_0);
	// There are up to 16 peripherals that can use this pin, so we need
	// to configure the multiplexer to choose the SPI. This uses a 64-bit
	// register pair called AFRH + AFRL. See '401 Reference Manual p 161, 
	// which tells us which bits of AFRH or AFRL control the muxes for PA4, 
	// 5, and 7, and see the '401 Data Sheet for which mux line attaches 
	// to the SPI1 unit. NOTE - really, our 3 pins can only host 3 or 4
	// different peripherals -- most MUX options are unused on the '401.
	// SS(active low): Pin PA_5 to Alternate-function-5 (Ref Man) since 
	// Alt-fnc-5 = SPI1_SCK (Data sheet).
	// SCK @ PA5; PA5 is bits (23-20) of ARFL (config to alt-fnc #5 = SPI)
	GPIOA->AFR[0] |= 5 << 20;
	// MOSI @ Pin PA_7 uses bits (31-28) of AFRL. To configure alt-fnc #5:
	GPIOA->AFR[0] |= 5 << 28;
	// For debugging -- it's eaier if we use a pin to signal "begin packet"		
	// GPIO PA4 will be my "enable" line; HIGH = Between packets.
	// It should be a GPIO output; I picked PA4.
	GPIOA->MODER |= (GPIO_MODER_MODER4_0);
	GPIOA->MODER &= ~(GPIO_MODER_MODER4_1);
	// Set the drivers to exert themselves to give crisp edges at high freq.
	// Yes, the bits masks have a spelling error with an extra E...
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR4_0); // Sharp output edges.
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR5_0); // Sharp output edges.
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR7_1 | GPIO_OSPEEDER_OSPEEDR7_0); // Sharp output edges.
	// See Reference (6.3.2) on page 121 to get a clock to the SPI1 config unit.
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	// See Reference (20.5) on pages 597ff for SPI configuration:
	// SPI = 16 bits (DFF =1),
	// MSB first (LSBFIRST = 0)
	// Mode(0,0) (CPOL = 0, CPHA = 0)
	// Slave-select-via-SW, Master = Me,SS' pin high (SSM = 1, MSTR = 1, SSI = 1)
	// SCK rate = (APB2-clock = ___)/256 => _____ (here, /256 because BR = 110)		
	// In this program, no change was made to RCC->CFGR, so APB2 @ 16 MHz,
	// thus SCK freq = 16 MHz/256 = 62 500 Hz
	// RCC->CFGR |= RCC_CFGR_PPRE2_2; // Divide APB2 to 42 MHz
	SPI1->CR1 = 0;
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI 
						| SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
						| SPI_CR1_MSTR;
	SPI1->CR1 &=~	SPI_CR1_DFF;		// Start w/ clean status;
	SPI1->SR = 0;						// Ready, so Enable peripheral.
	SPI1->CR1 |= SPI_CR1_SPE;
	int Initialize[13] = {0x40,0xA1,0xC0,0xA4,0xA6,0xA2,0x2F,0x27,0x81,0x10,0xFA,0x90,0xAF};
	GPIOA ->ODR &=~ GPIO_ODR_ODR_6; 	// nReset output = 0; 
	GPIOA ->ODR |= GPIO_ODR_ODR_1; 		//	nCS0 output = 1; 
	for(uint16_t i=0; i<4000;i++){;}	//wait 1ms
	GPIOA ->ODR |= GPIO_ODR_ODR_6; 		//	nReset output = 1;
	for(uint16_t i=0; i<18000;i++){;} 	// wait 5ms	
	GPIOA ->ODR &=~ GPIO_ODR_ODR_1; 	// nCS0 output = 0;	
	for(uint16_t i=0; i<18000; i++){;} 	// wait 5ms
	uint16_t abcd=0;
	while(abcd<13){
	//check_spi();	
		if (~SPI1->SR & SPI_SR_BSY) {	
			for (int32_t i = 0; i<100; i++); 
			for (int32_t i = 0; i < 1000; i++){	
				GPIOA->ODR |= GPIO_ODR_OD4; 
			}
			GPIOA->ODR &= ~GPIO_ODR_OD4;
			SPI1->DR = Initialize[abcd]; 
			abcd++;
		}
	}
	for (uint64_t i = 0; i<1000000; i++){;} 
}

void init_spi(void){
	  // Enable GPIO-A control (required by SCK = PA_5, MOSI = PA_7)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
	
	  // Tell the GPIO multiplexer to connect PB5, 4, 3 to their alternate fnc.
	  // Reference Guide, page 157
    GPIOB->MODER &=~ (GPIO_MODER_MODER5_0); // SPI3 MOSI
    GPIOB->MODER |=  (GPIO_MODER_MODER5_1);
	
	  GPIOB->MODER &=~ (GPIO_MODER_MODER3_0); // SPI3 SCK
    GPIOB->MODER |=  (GPIO_MODER_MODER3_1);
	
	  GPIOB->MODER &=~ (GPIO_MODER_MODER13_0); // SPI2 SCK
    GPIOB->MODER |=  (GPIO_MODER_MODER13_1);
	
	  GPIOB->MODER &=~ (GPIO_MODER_MODER15_0); // SPI2 MOSI
    GPIOB->MODER |=  (GPIO_MODER_MODER15_1);
	
		GPIOA->MODER &=~ (GPIO_MODER_MODER15_0); // SPI3 SS
		GPIOA->MODER |=  (GPIO_MODER_MODER15_1);
	
		GPIOC->MODER &=~ (GPIO_MODER_MODER1_1); 		// PC1 is GPIO to speaker
		GPIOC->MODER |=	(GPIO_MODER_MODER1_0);
	
		GPIOC->MODER &=~ (GPIO_MODER_MODER0_1); 		// PC0 is GPIO to speaker
		GPIOC->MODER |=	(GPIO_MODER_MODER0_0);

		// SCK @ PA5; PB5 is bits (23-20) of ARFL (config to alt-fnc #5 = SPI) sclock
    GPIOB->AFR[0] |= 6 << 20;
	
	  // MOSI @ Pin PB_4 uses bits (19-16) of AFRL. To configure alt-fnc #5: sdata
    GPIOB->AFR[0] |= 6 << 16;
	
	  //MISO @ Pin PB3 uses bits (15-12) of AFRL. To configure alt-func #6: smiso
	  GPIOB->AFR[0] |= 6 << 12;
		
		// NSS @ Pin PA15 uses bits (31-28) of AFRH. To configure alt-func #6: NSS
		GPIOA->AFR[1] |= 6 << 28;
		
		// SPI2 SCK @ PB13 is bits (23-20) of AFRH. To configure alt-func #5
		GPIOB->AFR[1] |= 5 << 20;
		
		//SPI2 MOSI @ PB15 is bits (31-28) of AFRH. To configure alt-func #5
		GPIOB->AFR[1] |= 5 << 28;
	
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR3_0); // Sharp output edges.
    //GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR4_0); // Sharp output edges.
		GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR5_0); // Sharp output edges.
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR13_0); // Sharp output edges.
		GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR15_1 | GPIO_OSPEEDER_OSPEEDR15_0); // Sharp output edges.
		GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR15_1 | GPIO_OSPEEDER_OSPEEDR15_0); // Sharp output edges.
		GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR10_0); // Sharp output edges.
		GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2_1 | GPIO_OSPEEDER_OSPEEDR2_0); // Sharp output edges.
		GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR3_0); // Sharp output edges.
		
		// See Reference (6.3.2) on page 121 to get a clock to the SPI1 config unit.
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
		
		// See Reference (20.5) on pages 597ff for SPI configuration:
    // SPI = 16 bits (DFF =1),
    // MSB first (LSBFIRST = 0)
    // Mode(1,1) (CPOL = 1, CPHA = 1)
    // Slave-select-via-SW, Master = Me,SS' pin high (SSM = 1, MSTR = 1, SSI = 1)
    // SCK rate = (APB2-clock = ___)/256 => _____ (here, /256 because BR = 110)    
	  // In this program, no change was made to RCC->CFGR, so APB2 @ 16 MHz,
	  // thus SCK freq = 16 MHz/256 = 62 500 Hz
	  RCC->CFGR |= RCC_CFGR_PPRE2_2; // Divide AHB by 8, and APB2 by 2
    SPI3->CR1 = 0;
    SPI3->CR1 |= SPI_CR1_RXONLY | SPI_CR1_SSM
              | SPI_CR1_CPOL | SPI_CR1_CPHA;
		SPI3->CR1 &=~ SPI_CR1_SSI &~ SPI_CR1_DFF &~ SPI_CR1_MSTR;
		SPI2->CR1 = 0;
    SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI 
              | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
              | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
		// Start w/ clean status;
    SPI3->SR = 0;
		SPI2->SR = 0;
    // Ready, so Enable peripheral.
    SPI3->CR1 |= SPI_CR1_SPE;
		SPI2->CR1 |= SPI_CR1_SPE;
}
