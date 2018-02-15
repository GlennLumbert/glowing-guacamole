//Glenn Lumbert - Lab 3 - 2/10/18 - LCD keyboard initialization- clock speed = 16mhz
#include "stm32l476xx.h"
#include "lcd.h"
#include <math.h>
void System_Clock_Init(void);
void wait(void); 
unsigned char keypad_scan(void);
void LCD_DisplayString(uint8_t* ptr);
unsigned char key; 
unsigned char str[50];
unsigned char str_len = 0; 
unsigned char key_map[4][4] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'},
	};
int main(void) {
	char lstPressed = 0;	//char for the # function to record last pressed
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	LCD_Initialization();
	GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12 | GPIO_PUPDR_PUPD13); //sets no pull up or pull down for PUPDR register
	LCD_Clear();
	while(1){	//keypad_scan for the book
			key = keypad_scan();
				switch (key) {
					case '*': // if * pressed DELETE
						str[str_len-1] = ' ';
					if(str_len > 0) {
						str_len --;
					}
					break; 
					case '#':	//if # display lass pressed char
						str[str_len] = lstPressed;
					if((lstPressed !=0)) {
					if(str[str_len + 1] != '\0') {
						str_len++; 
					}
					}
					break;
					default: 
						str[str_len] = key; 
						lstPressed = key;
						str[str_len +1] = 0; //null
					if(str_len<6) {
						str_len++;
					}
						if(str_len >= 48) { 
							str_len = 0; 
						}
		}
		LCD_DisplayString((uint8_t*) str);
	}
}
unsigned char keypad_scan(void) {
	int i; 
	unsigned char mask =0xF;	//mask for use in changing odr
	unsigned char key = 0xF;
	unsigned char row, col;
	
	GPIOE->ODR &= ~(GPIO_ODR_OD10| GPIO_ODR_OD11 | GPIO_ODR_OD12 | GPIO_ODR_OD13); //sets rows to 0000
	for(i = 0; i < 500 ; i++); //short delay
	
	while ((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)); //waits for a key press
	//finds which column is being pressed
		if ((GPIOA->IDR & (GPIO_IDR_ID2|GPIO_IDR_ID3|GPIO_IDR_ID5)) == (GPIO_IDR_ID2|GPIO_IDR_ID3|GPIO_IDR_ID5)) {   
		col = 0;
		}
		if ((GPIOA->IDR & (GPIO_IDR_ID1|GPIO_IDR_ID3|GPIO_IDR_ID5)) == (GPIO_IDR_ID1|GPIO_IDR_ID3|GPIO_IDR_ID5)) {
			col = 1;
		}
		if ((GPIOA->IDR & (GPIO_IDR_ID2|GPIO_IDR_ID1|GPIO_IDR_ID5)) == (GPIO_IDR_ID2|GPIO_IDR_ID1|GPIO_IDR_ID5)) {
			col = 2;
		}
		if ((GPIOA->IDR & (GPIO_IDR_ID2|GPIO_IDR_ID3|GPIO_IDR_ID1)) == (GPIO_IDR_ID2|GPIO_IDR_ID3|GPIO_IDR_ID1)) {
			col = 3;
		}
		
	for(row = 0; row <4; row++) {
			GPIOE->ODR &= ~(GPIO_ODR_OD10| GPIO_ODR_OD11 | GPIO_ODR_OD12 | GPIO_ODR_OD13); //sets rows to 0000
			GPIOE->ODR |= (((mask - ((unsigned char) pow(2,row))) << 10));
			for(i = 0; i < 500 ; i++); //short delay
		if((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) != (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) {
			while (!((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)));
		return key_map[row][col];
		}
	}
	return key; 
}

void System_Clock_Init(void){
	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );
		
}

