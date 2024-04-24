// kate: line-numbers on; tab-indents on; tab-width 2;

#ifndef _HEART_BIT
	#define _HEART_BIT
	
	#include <avr/io.h>
	#include <avr/interrupt.h>
	
	#define HEART_ON_1_TIME					10			// x10 ms
	#define HEART_OFF_TIME					10			// x10 ms
	#define HEART_ON_2_TIME					30			// x10 ms
	#define HEART_IDLE_TIME					100			// x10 ms
	
	#define STATE_IDLE							0x00
	#define STATE_BIT_1							0x01
	#define STATE_BIT_SEPARATION		0x02
	#define STATE_BIT_2							0x03
	
	void heart_bit_init( void );
	void enable_heart_bit( uint8_t enabled );
	
#endif
