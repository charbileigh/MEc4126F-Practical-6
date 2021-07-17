// Name: Francesca Seopa
// Student number: SPXMMA001
// MEC4126F: Practial  6, Date: 22 June 2021





// The Purpose of the Programme----------------------------------------------------------------|
/*
 * This prac will familiarise you with how to generate PWM signals on your STM32 and
 * how to update the duty cycle in response to an external command. 
 * This provides the foundational material for implementing the final step of the line 
 * following robot where motors must be controlled in various ways in response to sensor inputs.
 * 
 */
//---------------------------------------------------------------------------------------------|









// Description----------------------------------------------------------------|
/*
 * Initialises a struct with Name and Age data. Displays results on LEDs and
 * LCD.
 */
// DEFINES AND INCLUDES-------------------------------------------------------|

#define STM32F051
//>>> Uncomment line 10 if using System Workbench (SW4STM32) or STM32CubeIDE
//#define SW4STM32

#ifndef SW4STM32
	#define TRUESTUDIO
#endif

#include "stm32f0xx.h"
#include "lcd_stm32f0-1.h"
// GLOBAL VARIABLES ----------------------------------------------------------|
unsigned char buffer[16]; 
uint8_t ADC_value;
// FUNCTION DECLARATIONS -----------------------------------------------------|

void main(void);                                                   //COMPULSORY
void init_ADC(void);
void init_LEDs(void);
void init_buttons(void);
void init_external_interrupts(void);
void EXTI2_3_IRQHandler(void);
void display_on_LEDs(void);
void display_on_LCD(void);

#ifdef TRUESTUDIO												   //COMPULSORY
	void reset_clock_to_48Mhz(void);							   //COMPULSORY
#endif															   //COMPULSORY



// MAIN FUNCTION -------------------------------------------------------------|

void main(void)
{
#ifdef TRUESTUDIO  											 	   //COMPULSORY
	reset_clock_to_48Mhz();										   //COMPULSORY
#endif															   //COMPULSORY
	init_LCD();
	init_ADC();
	init_LEDs();

	while(1)
	{
		display_on_LCD();
		display_on_LEDs();
		delay(500);
	}
}

// OTHER FUNCTIONS -----------------------------------------------------------|
// continuous conversion
void init_ADC(void){
	//Configuraring the GPIOA Pin 5
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // enable clock PORT A
	GPIOA->MODER |= GPIO_MODER_MODER5; // set PA5, analogue mode
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // enable clock for ADC

	// Configuraring the ADC
	ADC1->CR &= ~ADC_CR_ADSTART; // stop ADC
	ADC1->CFGR1 |= ADC_CFGR1_RES_1; // 8 bit resolution
	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN; // right alignment
	ADC1->CFGR1 &= ~ADC_CFGR1_CONT; // continuous conversion mode (double check here)
	ADC1->CHSELR |= ADC_CHSELR_CHSEL5; // Channel 5 = PA5

	ADC1->CR |= ADC_CR_ADEN; // sets ADEN to 1 in ADC_CR register

	while((ADC1->ISR & ADC_ISR_ADRDY) == 0); // exits loop when ADRDY == 1
	ADC1 -> CR |= ADC_CR_ADSTART; // sets ADSTART to 1 in ADC_CR

	while((ADC1->ISR & ADC_ISR_EOC) == 0); // exits loop when EOC == 1
	ADC_value = ADC1->DR; // write value to ADC_value variable
}

// single conversion
// void init_ADC(void){
// 	RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // enable clock for ADC

// 	// Configuraring the ADC
// 	ADC1->CR &= ~ADC_CR_ADSTART; // stop ADC
// 	ADC1->CFGR1 |= ADC_CFGR1_RES_1; // 8 bit resolution
// 	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN; // right alignment
// 	ADC1->CFGR1 &= ~ADC_CFGR1_CONT; // single conversion mode (double check here)
// 	ADC1->CHSELR |= ADC_CHSELR_CHSEL5; // Channel 5 = PA5

// 	ADC1->CR |= ADC_CR_ADEN; // sets ADEN to 1 in ADC_CR register

// 	while((ADC1->ISR & ADC_ISR_ADRDY) == 0); // exits loop when ADRDY == 1
// 	ADC1 -> CR |= ADC_CR_ADSTART; // sets ADSTART to 1 in ADC_CR

// 	while((ADC1->ISR & ADC_ISR_EOC) == 0); // exits loop when EOC == 1
// 	ADC_value = ADC1->DR; // write value to ADC_value variable
// }

void display_on_LCD(void){
	sprintf(buffer, "%d", ADC_value);
	lcd_command(CLEAR);
	lcd_putstring(buffer);
}

void init_LEDs(void){
	// Enable clock for port B
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// set pins PB0-PB7 to GPIO outputs
	GPIOB->MODER |= GPIO_MODER_MODER0_0;
	GPIOB->MODER |= GPIO_MODER_MODER1_0;
	GPIOB->MODER |= GPIO_MODER_MODER2_0;
	GPIOB->MODER |= GPIO_MODER_MODER3_0;
	GPIOB->MODER |= GPIO_MODER_MODER4_0;
	GPIOB->MODER |= GPIO_MODER_MODER5_0;
	GPIOB->MODER |= GPIO_MODER_MODER6_0;
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
}

void display_on_LEDs(void){
	GPIOB->ODR = ADC_value;
}

// void init_buttons(void){
// 	// Enable clock for Port A
// 	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
// 	// Enable input mode for PA3
// 	GPIOA->MODER &= ~GPIO_MODER_MODER3;
// 	// Enable pull up resistor for PA3
// 	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0; 
// }

// void init_external_interrupts(void){
// 	// Enable clock
// 	RCC->AHBENR |= RCC_APB2ENR_SYSCFGCOMPEN;
// 	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;
// 	EXTI->IMR |= EXTI_IMR_MR3;
// 	EXTI->FTSR |= EXTI_FTSR_TR1;

// 	NVIC_EnableIRQ(EXTI2_3_IRQn);
// }

void EXTI2_3_IRQHandler(void){
	if (EXIT->PR &= EXIT_PR_PRO){
		// display LEDS
		display_on_LEDs();
		// display LCD
		display_on_LCD();
	}
	EXIT->PR |= EXIT_PR_PR0;
}

#ifdef TRUESTUDIO												   //COMPULSORY
/* Description:
 * This function resets the STM32 Clocks to 48 MHz
 */
void reset_clock_to_48Mhz(void)									   //COMPULSORY
{																   //COMPULSORY
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)			   //COMPULSORY
	{															   //COMPULSORY
		RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW);					   //COMPULSORY
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);	   //COMPULSORY
	}															   //COMPULSORY

	RCC->CR &= (uint32_t)(~RCC_CR_PLLON);						   //COMPULSORY
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);						   //COMPULSORY
	RCC->CFGR = ((RCC->CFGR & (~0x003C0000)) | 0x00280000);		   //COMPULSORY
	RCC->CR |= RCC_CR_PLLON;									   //COMPULSORY
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);						   //COMPULSORY
	RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL);					   //COMPULSORY
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);		   //COMPULSORY
}																   //COMPULSORY
#endif															   //COMPULSORY






// INTERRUPT HANDLERS --------------------------------------------------------|




