// kate: line-numbers on; tab-indents on; tab-width 2;

#include "ios_def.h"

void configure_ios( void ) {
	// Default values
	PORTA.OUT			= PIN0_bm |
									PIN3_bm |
									PIN4_bm |
									PIN5_bm |
									PIN7_bm;
	PORTB.OUT			= 0x00;
	PORTC.OUT			= 0x00;
	PORTD.OUT			= PIN1_bm;
	PORTE.OUT			= 0x00;
	PORTF.OUT			= 0x00;
	
	
	PORTA.DIR			= PIN0_bm |
									PIN1_bm |
									PIN2_bm |
									PIN3_bm |
									PIN4_bm |
									PIN5_bm |
									PIN7_bm;
	
	PORTB.DIR			= 0xFF;				// All outputs
	
	PORTC.DIR			= 0x00;				// All inputs (dynamically changed to outputs)
	
	PORTD.DIR			= PIN1_bm |
									PIN3_bm |
									PIN5_bm |
									PIN7_bm;
	
	PORTE.DIR			= 0xFF;				// All outputs
	
	PORTF.DIR			= 0x00;				// All inputs
}
