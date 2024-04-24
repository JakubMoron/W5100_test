// kate: line-numbers on; tab-indents on; tab-width 2;

#include "heart_bit.h"

//----------------------------------------------------
// PORT C Timer 1 config
// Period = 10 ms
//----------------------------------------------------
void timer_C1_config(void){
	TCC1.CTRLB			=	TC_WGMODE_NORMAL_gc;				// normal mode
	TCC1.PER				=	39999;											// overflow value ( 32.000.000 / 8 = 0.25us * 40.000 = 10ms )
	TCC1.INTCTRLA		=	TC_OVFINTLVL_LO_gc;					// overflow - Low priority interrupt
}

uint16_t heart_bit_counter;
uint8_t heart_bit_state;

ISR( TCC1_OVF_vect ) {
	switch( heart_bit_state ){
		
		case STATE_IDLE:
			if( ++heart_bit_counter == HEART_IDLE_TIME ) {
				heart_bit_counter = 0;
				heart_bit_state = STATE_BIT_1;
				PORTA.OUTSET = PIN2_bm;
			}
			break;
		
		case STATE_BIT_1:
			if( ++heart_bit_counter == HEART_ON_1_TIME ) {
				heart_bit_counter = 0;
				heart_bit_state = STATE_BIT_SEPARATION;
				PORTA.OUTCLR = PIN2_bm;
			}
			break;
		
		case STATE_BIT_SEPARATION:
			if( ++heart_bit_counter == HEART_OFF_TIME ) {
				heart_bit_counter = 0;
				heart_bit_state = STATE_BIT_2;
				PORTA.OUTSET = PIN2_bm;
			}
			break;
		
		case STATE_BIT_2:
			if( ++heart_bit_counter == HEART_ON_2_TIME ) {
				heart_bit_counter = 0;
				heart_bit_state = STATE_IDLE;
				PORTA.OUTCLR = PIN2_bm;
			}
			break;
	}
}

void heart_bit_init( void ) {
	heart_bit_counter = 0;
	heart_bit_state = STATE_IDLE;
	
	timer_C1_config();
}


void enable_heart_bit( uint8_t enabled ){
	if( enabled )
		TCC1.CTRLA			=	TC_CLKSEL_DIV8_gc;					// prescale set - timer on
	else
		TCC1.CTRLA			=	0;													// prescale unset - timer off
}
