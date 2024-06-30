#include "stm32f4xx.h" 

#define MODER_INPUT   (0b00) // Mode for input
#define MODER_OFFSET_13  (26U) // Offset for pin 13 (13 * 2)

#define MODER_OUTPUT   (0b01) // Mode for output
#define MODER_OFFSET_5  (10U) // Offset for pin 5 (5 * 2)

int mode = 0;

void EXTI15_10_IRQHandler(void) {
	if ((EXTI->PR & (1 << 13)) == 1) {
		EXTI->PR |= (1 << 13);
		mode++;
		if (mode > 1) mode = 0;
	}
}

int main(void) {
	// turn on the AHB1 Clock Enable Register of Port A
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
	
	// enable the interrupt
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// connect EXTI line to GPIOA
	SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI13_Msk); // GPIOA Pin 13 is reg 3
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PA;  // Set EXTI1 to use GPIOA
	
	// button interrupt configuration
	EXTI->IMR |= (1 << 13); // line 13 of EXTI1
	EXTI->RTSR &= ~(1 << 13); // no rising edge
	EXTI->FTSR |=  (1 << 13); // yes falling edge
	
	// NVIC
	NVIC_SetPriority(EXTI4_IRQn, 0x03);
	NVIC_EnableIRQ(EXTI4_IRQn);
	
	// configure the IO Pins
	GPIOA->MODER &= ~(0b00 << MODER_OFFSET_13); // Clear the two bits
  //GPIOA->MODER |= (MODER_INPUT << MODER_OFFSET_13); // Set the mode to input (0b00)
	
	GPIOA->MODER &= ~(0b01 << MODER_OFFSET_5); // Clear the two bits
	GPIOA->MODER |= (MODER_OUTPUT << MODER_OFFSET_5); // Set the mode to input (0b00)
	
	// endless loop
	while (1) {
		if (mode == 1) {
			GPIOA->BSRR = GPIO_BSRR_BS5; // (1U << 5)
		}
		else if (mode == 0) {
			GPIOA->BSRR = GPIO_BSRR_BR5; // (1U << 5)
		}
		EXTI15_10_IRQHandler();
		/*
		if ((GPIOA->IDR & (1U << 13)) >> 13 == 1) {
			GPIOA->BSRR = GPIO_BSRR_BS5; // (1U << 5)
		}
		*/
	}
}