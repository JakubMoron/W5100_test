// kate: line-numbers on; tab-indents on; tab-width 2;

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "ios_def.h"
#include "w5100.h"
#include "heart_bit.h"
#include "uart.h"

//----------------------------------------------------
// Turn on internal 32 MHz RC oscillator
//----------------------------------------------------
void osc32MHz(void) {
	OSC.CTRL	=	OSC_RC32MEN_bm;							// internal 32MHz RC oscil enable
	
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));	// wait for oscil to stabilize
	
	CPU_CCP		=	CCP_IOREG_gc;								// clk source change unlock (?)
	CLK.CTRL	=	CLK_SCLKSEL_RC32M_gc;				// change clk to internal RC 32MHz
}

//----------------------------------------------------
// PORT C Timer 0 config
// Main timer for system timing
// Period = 1 ms
//----------------------------------------------------
// void timer_C0_config(void){
// 	TCC0.CTRLB			=	TC_WGMODE_NORMAL_gc;				// normal mode
// 	TCC0.PER				=	3999;												// overflow value ( 32.000.000 / 8 = 0.25us * 4.000 = 1ms ). Clock prescaler is set in last command
// 	TCC0.INTCTRLA		=	TC_OVFINTLVL_HI_gc;					// overflow - High priority interrupt
// 	TCC0.CTRLA			=	TC_CLKSEL_DIV8_gc;					// prescale set - timer on
// }

//----------------------------------------------------
// Timer 0 overflow interrupt handler - every 1 ms
//----------------------------------------------------
// ISR( TCC0_OVF_vect ) {
// 	
// }

//----------------------------------------------------
// Turn on interrupts - all levels on
//----------------------------------------------------
void interrupt_config(void){
	PMIC.CTRL=
		PMIC_HILVLEN_bm |
		PMIC_MEDLVLEN_bm |
		PMIC_LOLVLEN_bm;
	sei();
}

int main( void ) {
	osc32MHz();
	
	configure_ios();
	interrupt_config();
	heart_bit_init();
	
	UART_D0_config();
	
	// Clear error led, turn on ok
	PORTA.OUTCLR = PIN0_bm;
	PORTA.OUTSET = PIN1_bm;
	
	w5100_init();
	
	/*
	// Dump W5100 registers
	char tx_buf[64];
	uint8_t data;
	
 	for(uint8_t i = 0; i<64; i++ ) {
 		data = w5100_read_register( i );
 		sprintf(tx_buf, "Reg 0x%02X = 0x%02X\n", i, data);
 		send_string( tx_buf );
 	}
 	*/
	
	enable_heart_bit( 1 );
	send_string( "WIZ811MJ on ATXmega 384 D3" );
	
	while( 1 ) {
		process_tcp_server();
		
		_delay_us( 1 );
	}
	
	
}


