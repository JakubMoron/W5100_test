// kate: line-numbers on; tab-indents on; tab-width 2;

#ifndef _UART
	#define _UART
	
	#include <avr/io.h>
	
	void UART_D0_config( void );
	void usart_putc( char c );
	void send_string( char* string );
	void send_progmem( const __flash char* string );
	
#endif
