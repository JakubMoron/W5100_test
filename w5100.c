// kate: line-numbers on; tab-indents on; tab-width 2;

#include "w5100.h"
#include "w5100_config.h"
#include "web_progmem.h"

#include "uart.h"
#include <stdio.h>
#include <string.h>


uint8_t data_buffer[ MAX_BUF ];

void w5100_init( void ) {
	uint8_t i;
	
	// Apply soft reset to the W5100
	w5100_write_register( MR_REG, 0x80 );
	_delay_ms( 1 );
	
	// Set up gateway address
	for( i=0; i<4; i++ )
		w5100_write_register( GAR_BASE + i, gtw_addr[ i ] );
	
	// Set up subnet mask 
	for( i=0; i<4; i++ )
		w5100_write_register( SUBR_BASE + i, sub_mask[ i ] );
	
	// Set up MAC address
	for( i=0; i<6; i++ )
		w5100_write_register( SAR_BASE + i, mac_addr[ i ] );
	
	// Set up IP address
	for( i=0; i<4; i++ )
		w5100_write_register( SIPR_BASE + i, ip_addr[ i ] );
	
	// Set up TX and RX memory size to 2kB for 4 TX/RX channels
	w5100_write_register( RMSR_REG, 0x55 );
	w5100_write_register( TMSR_REG, 0x55 );
}

void process_tcp_server( void ) {
	char tx_buf[64];
	static uint8_t last_status = 0x01;
	uint8_t socket_status = w5100_read_register( S0_SR_REG );
	uint8_t ret;
	uint16_t recv_size;
	
	if( last_status != socket_status ) {
		sprintf(tx_buf, "Socket status: 0x%02X\n", socket_status );
		send_string( tx_buf );
		last_status = socket_status;
	}
	
	
	switch( socket_status ) {
		
		case Sx_SR_CLOSED_gc:
			ret = open_socket( Sx_MR_TCP_gc, TCP_PORT ); 
			
			if( ret == STATUS_OK ) {
				send_string( "Socket opened\n" );
				
				ret = listen();
				if( ret == STATUS_OK ) {
					_delay_ms( 1 );
					send_string( "Socket listen\n" );
				} else {
					sprintf(tx_buf, "Socket listen error 0x%02X\n", ret );
					send_string( tx_buf );
				}
			} else {
				sprintf(tx_buf, "Socket open error 0x%02X\n", ret );
				send_string( tx_buf );
			}
			break;
		
		case Sx_SR_ESTABLISHED_gc:
			recv_size = get_recv_size();
			
			if( recv_size ) {
				
				sprintf(tx_buf, "Received size %d\n", recv_size );
				send_string( tx_buf );
				
				receive( data_buffer, recv_size );
				send_string( (char* )data_buffer );
				send_string( "\n --- PACKAGE END ---\n" );
				// Parse request here
				
				sprintf(tx_buf, "Send size %d\n", strlen_P( web_content ) );
				send_string( tx_buf );
				send_string( "\n --- Sending: ---\n" );
				send_progmem( web_content );
				
				ret = send( web_content, strlen_P( web_content ) );
				send_string( "\n --- SEND ISSUED ---\n" );
				
				if( ret == STATUS_OK )
					send_string( "Send OK!\n" );
				else {
					sprintf(tx_buf, "Send error 0x%02X\n", ret );
					send_string( tx_buf );
				}
				disconnect_socket();
			}
			break;
		
		case Sx_SR_CLOSE_WAIT_gc:
			send_string( "Disconnecting socket..." );
			disconnect_socket();
			send_string( "\n --- Socket disconnected ---\n" );
			break;
		
		case Sx_SR_FIN_WAIT_gc:
		case Sx_SR_CLOSING_gc:
		case Sx_SR_TIME_WAIT_gc:
		case Sx_SR_LAST_ACK_gc:
			// Force to close the socket
			close_socket();
			send_string( "Socket closed.\n" );
			break;
	}
}





void close_socket( void ) {
	// Send close socket command
	w5100_write_register( S0_CR_REG, Sx_CR_CLOSE_gc );
	
	// Wait for close process to complete
	wait_for_CR_clear();
}

void disconnect_socket( void ) {
	// Send disconnect socket command
	w5100_write_register( S0_CR_REG, Sx_CR_DISCON_gc );
	
	// Wait for disconnect process to complete
	wait_for_CR_clear();
}

uint8_t open_socket( uint8_t protocol, uint16_t port ) {
	// Make sure we close the socket first
	if( w5100_read_register( S0_SR_REG ) != Sx_SR_CLOSED_gc )
		close_socket();
	
	// Set ethernet protocol in MR register
	w5100_write_register( S0_MR_REG, protocol );
	
	// Set port
	w5100_write_register( S0_PORT_BASE    , (uint8_t)( port >> 8 ) );
	w5100_write_register( S0_PORT_BASE + 1, (uint8_t)( port ) );
	
	// Open socket
	w5100_write_register( S0_CR_REG, Sx_CR_OPEN_gc );
	
	// Wait for socket open process to finish
	wait_for_CR_clear();
	
	// Read socket status
	uint8_t socket_status = w5100_read_register( S0_SR_REG );
	
	if( socket_status != Sx_SR_INIT_gc ) {
		close_socket();	
		return socket_status;
	}
	
	return STATUS_OK;
}

uint8_t listen( void ) {
	uint8_t socket_status = w5100_read_register( S0_SR_REG );
	
	if ( socket_status == Sx_SR_INIT_gc ) {
		// Send the LISTEN Command
		w5100_write_register( S0_CR_REG, Sx_CR_LISTEN_gc );
		
		// Wait for listen command to execute
		wait_for_CR_clear();
		
		// Read socket status
		socket_status = w5100_read_register( S0_SR_REG );
		
		if( socket_status != Sx_SR_LISTEN_gc ) {
			close_socket();
			return socket_status;
		}
		
		return STATUS_OK;
	}
	
	return socket_status;
}

uint8_t send( const __flash char* buf, uint16_t buf_len ) {
	uint16_t write_ptr, write_addr, tx_size, timeout;
	
	// Make sure the TX Free Size Register is available
	tx_size = w5100_read_register( S0_TX_FSR_BASE ) << 8;
	tx_size |= w5100_read_register( S0_TX_FSR_BASE + 1 );
	
	// Wait for TX free size to fit buffer
	timeout = 0;
	while( tx_size < buf_len ) {
		_delay_ms( 1 );
		
		tx_size = w5100_read_register( S0_TX_FSR_BASE ) << 8;
		tx_size |= w5100_read_register( S0_TX_FSR_BASE + 1 );
		
		if( timeout++ > TX_FREE_TIMEOUT ) {
			disconnect_socket();
			return ERR_SEND_TIMEOUT;
		}
	}
	
	// Read the Tx Write Pointer
	write_ptr = w5100_read_register( S0_TX_WR_BASE ) << 8;
	write_ptr |= w5100_read_register( S0_TX_WR_BASE + 1 );
	
	// Copy buffer to the TX memory
	while( buf_len-- ) {
		// Calculate the real W5100 physical Tx Buffer Address
		write_addr = TX_BUF_BASE_ADDR + ( write_ptr & TX_BUF_MASK );
		
		// Copy the application data to the W5100 Tx Buffer
		w5100_write_register( write_addr, *buf );
		
		write_ptr++;
		buf++;
	}
	
	// Increase the S0_TX_WR value, so it point to the next transmit
	w5100_write_register( S0_TX_WR_BASE    , (uint8_t)( write_ptr >> 8 ) );
	w5100_write_register( S0_TX_WR_BASE + 1, (uint8_t)( write_ptr ) );
	
	// Send the send command
	w5100_write_register( S0_CR_REG, Sx_CR_SEND_gc );
	
	// Wait for send command to complete
	wait_for_CR_clear();
	
	return STATUS_OK;
}

void receive( uint8_t* buf, uint16_t buf_len ) {
	uint16_t read_ptr, read_addr;
	
	// Read the Rx Read Pointer
	read_ptr = w5100_read_register( S0_RX_RD_BASE ) << 8;
	read_ptr |= w5100_read_register( S0_RX_RD_BASE + 1 );
	
	while( buf_len-- ) {
		// Calculate the real W5100 physical Rx Buffer Address
		read_addr = RX_BUF_BASE_ADDR + ( read_ptr & RX_BUF_MASK );
		
		*buf = w5100_read_register( read_addr );
		
		read_ptr++;
		buf++;
	}
	// Add string termination character
	*buf = '\0';
	
	// Increase the S0_RX_RD value, so it point to the next receive
	w5100_write_register( S0_RX_RD_BASE    , (uint8_t)( read_ptr >> 8 ) );
	w5100_write_register( S0_RX_RD_BASE + 1, (uint8_t)( read_ptr ) );
	
	// Send the RECV command
	w5100_write_register( S0_CR_REG, Sx_CR_RECV_gc );
	
	// Wait for Receive Process
	_delay_us(5);
}


uint16_t get_recv_size( void ) {
	uint16_t recv_size = w5100_read_register( S0_RX_RSR_BASE ) << 8;
	recv_size |= w5100_read_register( S0_RX_RSR_BASE + 1 );
	return recv_size;
}




uint8_t w5100_read_register( uint16_t reg_address ) {
	uint8_t data;
	
	// Set address
	PORTB.OUT = (uint8_t) reg_address;
	PORTE.OUT = (uint8_t) ( reg_address >> 8 );
	
	// CS low
	PORTA.OUTCLR = PIN7_bm;
	
	// RD low
	PORTA.OUTCLR = PIN4_bm;
	
	_delay_us( 1 );
	
	// Read data
	data = PORTC.IN;
	
	// RD high
	PORTA.OUTSET = PIN4_bm;
	
	// CS high
	PORTA.OUTSET = PIN7_bm;
	
	return data;
}


void w5100_write_register( uint16_t reg_address, uint8_t data ) {
	// Set address
	PORTB.OUT = (uint8_t) reg_address;
	PORTE.OUT = (uint8_t) ( reg_address >> 8 );
	
	// Write data
	PORTC.OUT = data;
	
	// Set data as output
	PORTC.DIR = 0xFF;
	
	// CS low
	PORTA.OUTCLR = PIN7_bm;
	
	// WR low
	PORTA.OUTCLR = PIN5_bm;
	
	_delay_us( 1 );
	
	// WR high
	PORTA.OUTSET = PIN5_bm;
	
	// Set data as input
	PORTC.DIR = 0x00;
	
	// CS high
	PORTA.OUTSET = PIN7_bm;
}

