// kate: line-numbers on; tab-indents on; tab-width 2;

#ifndef _IOS_DEF
	#define _IOS_DEF
	
	#include <avr/io.h>
	
	/* -------------------------------------------------------------------------------------------------
	PORT MAP
	
	PORT A:
			0:	xM_STAT_OK					OUT					// Red diode - wrong LED assembled
			1:	xM_STAT_ERR					OUT					// Green diode - wrong LED assembled
			2:	xM_HEART_BIT				OUT
			3:	ETH_nRESET					OUT
			4:	ETH_nRD							OUT
			5:	ETH_nWR							OUT
			6:	ETH_nINT						IN
			7:	ETH_nMCS						OUT
		
		PORT B:
			0:	ETH_A0							OUT
			1:	ETH_A1							OUT
			2:	ETH_A2							OUT
			3:	ETH_A3							OUT
			4:	ETH_A4							OUT
			5:	ETH_A5							OUT
			6:	ETH_A6							OUT
			7:	ETH_A7							OUT
		
		PORT C:
			0:	ETH_D0							IN/OUT
			1:	ETH_D1							IN/OUT
			2:	ETH_D2							IN/OUT
			3:	ETH_D3							IN/OUT
			4:	ETH_D4							IN/OUT
			5:	ETH_D5							IN/OUT
			6:	ETH_D6							IN/OUT
			7:	ETH_D7							IN/OUT
		
		PORT D:
			0:	nc									IN
			1:	ETH_CS							OUT
			2:	xM_MASTER_RXD				IN
			3:	xM_MASTER_TXD				OUT
			4:	nc									IN
			5:	ETH_MOSI						OUT
			6:	ETH_MISO						IN
			7:	ETH_SCK							OUT
		
		PORT E:
			0:	ETH_A8							OUT
			1:	ETH_A9							OUT
			2:	ETH_A10							OUT
			3:	ETH_A11							OUT
			4:	ETH_A12							OUT
			5:	ETH_A13							OUT
			6:	ETH_A14							OUT
			7:	nc
		
		PORT F:
			0:	DBG0								IN/OUT
			1:	DBG1								IN/OUT
			2:	DBG2								IN/OUT
			3:	DBG3								IN/OUT
			4:	DBG4								IN/OUT
			5:	DBG5								IN/OUT
			6:	DBG6								IN/OUT
			7:	DBG7								IN/OUT
		
	------------------------------------------------------------------------------------------------- */
	
	void configure_ios( void );
	
#endif
