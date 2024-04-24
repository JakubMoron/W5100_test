// kate: line-numbers on; tab-indents on; tab-width 2;

#ifndef _W5100_CONFIG
	#define _W5100_CONFIG
	
	const __flash uint8_t mac_addr[]	= { 0x00, 0x08, 0xDC, 0x67, 0x37, 0x1E };
	const __flash uint8_t ip_addr[]		= { 192, 168,   0,  10 };
	const __flash uint8_t sub_mask[]	= { 255, 255, 255,   0 };
	const __flash uint8_t gtw_addr[]	= { 192, 168,   0,   1 };
	
#endif
