// kate: line-numbers on; tab-indents on; tab-width 2;

#include "uart.h"

//----------------------------------------------------
// UART0 PORT.D config
//----------------------------------------------------
void UART_D0_config( void ) {
	/* Datasheet
	 * Fosc = 32 MHz
	 * B = 115 200
	 * 
	 * CLK2X = 0
	 * BSEL = 131
	 * BSCALE = -3
	 * err = -0.1
	 * 
	 * BAUDCTRLA = BSEL[7:0]
	 * BAUDCTRLB = BSCALE[3:0], BSEL[11:8]
	 * BSCALE is in u2: -3 is 0b1101
	 * 
	*/ 
	 
	USARTD0_BAUDCTRLB = ( 0xD << 4 );
	USARTD0_BAUDCTRLA = 131;
	
	USARTD0_CTRLC = USART_CHSIZE_8BIT_gc;
	USARTD0_CTRLA = USART_RXCINTLVL_HI_gc;
	USARTD0_CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	PMIC.CTRL |= PMIC_LOLVLEX_bm;
}


void usart_putc( char c ) {
	while (!(USARTD0_STATUS & USART_DREIF_bm));
	USARTD0.DATA = c;
}

char usart_rx_tx( void ) {
	while(!(USARTD0_STATUS & USART_RXCIF_bm));
	
	char ch=USARTD0_DATA; //receive character from user
	
	while(!(USARTD0_STATUS & USART_DREIF_bm));
	
	USARTD0_DATA=ch; //transmit the same character on terminal
	
	return ch;
}

void send_string( char* string ) {
	while( *string ) {
 		while(!(USARTD0_STATUS & USART_DREIF_bm));
		USARTD0.DATA = *string;
		string++;
	}
}

void send_progmem( const __flash char* string ) {
	while( *string ) {
 		while(!(USARTD0_STATUS & USART_DREIF_bm));
		USARTD0.DATA = *string;
		string++;
	}
}
