// kate: line-numbers on; tab-indents on; tab-width 2;

#ifndef _W5100
	#define _W5100
	
	#include <avr/io.h>
	#include <util/delay.h>
	#include <avr/pgmspace.h>
	
	
	// *****************************************************************************************
	// W5100 registers definition
	// *****************************************************************************************
	
	// --- General registers ---
	#define MR_REG								0x0000		// Mode Register
	#define GAR_BASE							0x0001		// Gateway Address:									0:3
	#define SUBR_BASE							0x0005		// Subnet mask Address:							0:3
	#define SAR_BASE							0x0009		// Source Hardware Address (MAC):		0:5
	#define SIPR_BASE							0x000F		// Source IP Address:								0:3
	#define RMSR_REG							0x001A		// RX Memory Size Register
	#define TMSR_REG							0x001B		// TX Memory Size Register
	
	// --- Socket 0 ---
	#define S0_MR_REG							0x0400		// Mode Register
	#define S0_CR_REG							0x0401		// Command Register
	#define S0_IR_REG							0x0402		// Interrupt Register
	#define S0_SR_REG							0x0403		// Status Register
	#define S0_PORT_BASE					0x0404		// Source Port											0:1
	#define S0_TX_FSR_BASE				0x0420		// Tx Free Size 										0:1
	#define S0_TX_RD_BASE					0x0422		// Tx Read Pointer									0:1
	#define S0_TX_WR_BASE					0x0424		// Tx Write Pointer									0:1
	#define S0_RX_RSR_BASE				0x0426		// Rx Received Size Pointer					0:1
	#define S0_RX_RD_BASE					0x0428		// Rx Read Pointer									0:1
	
	// --- TX and RX buffers address ---
	#define TX_BUF_BASE_ADDR			0x4000		// Send Buffer Base Address
	#define RX_BUF_BASE_ADDR			0x6000		// Read Buffer Base Address
	
	#define TX_BUF_MASK						0x07FF		// Tx 2K Buffer Mask:
	#define RX_BUF_MASK						0x07FF		// Rx 2K Buffer Mask:
	#define NET_MEMALLOC					0x05			// Use 2K of Tx/Rx Buffer
	
	// --- Socket mode register values ---
	// Protocol
	#define Sx_MR_CLOSE_gc				0x00			// Unused socket
	#define Sx_MR_TCP_gc					0x01			// TCP
	#define Sx_MR_UDP_gc					0x02			// UDP
	#define Sx_MR_IPRAW_gc				0x03			// IP LAYER RAW SOCK
	#define Sx_MR_MACRAW_gc				0x04			// MAC LAYER RAW SOCK
	#define Sx_MR_PPPOE_gc				0x05			// PPPoE
	// 
	#define Sx_MR_ND_bm						0x20			// No Delayed Ack(TCP) flag
	#define Sx_MR_MULTI_bm				0x80			// Multicasting support 
	
	// --- Socked commands (CR register)
	#define Sx_CR_OPEN_gc					0x01			// Initialize or open socket
	#define Sx_CR_LISTEN_gc				0x02			// Wait connection request in tcp mode(Server mode)
	#define Sx_CR_CONNECT_gc			0x04			// Send connection request in tcp mode(Client mode)
	#define Sx_CR_DISCON_gc				0x08			// Send closing reqeuset in tcp mode
	#define Sx_CR_CLOSE_gc				0x10			// Close socket
	#define Sx_CR_SEND_gc					0x20			// Update Tx memory pointer and send data
	#define Sx_CR_SEND_MAC_gc			0x21			// Send data with MAC address, so without ARP process
	#define Sx_CR_SEND_KEEP_gc		0x22			// Send keep alive message
	#define Sx_CR_RECV_gc					0x40			// Update Rx memory buffer pointer and receive data
	
	// --- Socket status (SR register)
	#define Sx_SR_CLOSED_gc				0x00			// Closed
	#define Sx_SR_INIT_gc					0x13			// Init state
	#define Sx_SR_LISTEN_gc				0x14			// Listen state
	#define Sx_SR_SYNSENT_gc			0x15			// Connection state
	#define Sx_SR_SYNRECV_gc			0x16			// Connection state
	#define Sx_SR_ESTABLISHED_gc	0x17			// Success to connect
	#define Sx_SR_FIN_WAIT_gc			0x18			// Closing state
	#define Sx_SR_CLOSING_gc			0x1A			// Closing state
	#define Sx_SR_TIME_WAIT_gc		0x1B			// Closing state
	#define Sx_SR_CLOSE_WAIT_gc		0x1C			// Closing state
	#define Sx_SR_LAST_ACK_gc			0x1D			// Closing state
	#define Sx_SR_UDP_gc					0x22			// UDP socket
	#define Sx_SR_IPRAW_gc				0x32			// IP raw mode socket
	#define Sx_SR_MACRAW_gc				0x42			// MAC raw mode socket
	#define Sx_SR_PPPOE_gc				0x5F			// PPPOE socket
	
	// Software socket buffer size
	#define MAX_BUF								512
	
	// Wait for TX buffer free timeout
	#define TX_FREE_TIMEOUT				1000
	
	// TCP port
	#define TCP_PORT         80       // TCP/IP Port
	
	
	// Socket errors and status
	#define STATUS_OK							0x00
	#define ERR_SEND_TIMEOUT			0x01
	
	void w5100_init( void );
	
	void close_socket( void ) ;
	void disconnect_socket( void );
	uint8_t open_socket( uint8_t protocol, uint16_t port );
	uint8_t listen( void );
	uint8_t send( const __flash char* buf, uint16_t buf_len );
	void receive( uint8_t* buf, uint16_t buf_len );
	uint16_t get_recv_size( void );
	void process_tcp_server( void );
	
	
	uint8_t w5100_read_register( uint16_t reg_address );
	void w5100_write_register( uint16_t reg_address, uint8_t data );
	
	void inline wait_for_CR_clear( void ) {
		// Wait until S0_CR is clear
		while( w5100_read_register( S0_CR_REG ) );
	};
	
	
#endif
