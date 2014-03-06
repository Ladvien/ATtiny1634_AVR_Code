// UART Example on the ATtiny1634 using UART0.
// C. Thomas Brittain
// letsmakerobots.com
#define F_CPU 8000000UL		//AVR clock frequency in Hz, used by util/delay.h
#include <avr/io.h>			//Holds Pin and Port defines.
#include <util/delay.h>		//Needed for delay.
#include <avr/interrupt.h>  //Add the interrupt library; int. used for RX.




//Preprocessing of functions.  This allows us to initialize functions
//without having to put them before the main.
void uart_init (int BUADRATE);
void USART_Transmit( unsigned char data );
void Serial_Print(char *StringOfCharacters);
void LF();
void CR();

//Used to hold a received byte.
char receivedByte;

// Main
int main()
{

	
	//Setup received data LED.
	DDRA |= (1 << PINA6);
	
	PORTA ^= (1 << PINA6);
	_delay_ms(250);
	PORTA ^= (1 << PINA6);
	
	//Initialize the serial connection and pass it a desired baud rate.
	uart_init(9600);
	
	//Forever loop.
	while(1){
		
		//Call the Serial_Print function.
		Serial_Print(receiveData);
		_delay_ms(400);
		receiveData = 0;
	}
}

// function to initialize UART
void uart_init (int Desired_Baudrate)
{
	//Only set baud rate once.  If baud is changed serial data is corrupted.
	#ifndef UBBR
		//Set the baud rate dynamically, based on current microcontroller
		//speed and the desired baud rate given by the user.
		#define UBBR ((F_CPU)/(Desired_Baudrate*16UL)-1)
	#endif
		
	//Set baud rate.
	UBRR0H = (unsigned char)(UBBR>>8);
	UBRR0L = (unsigned char)UBBR;
	
	//Enables the RX interrupt.
	//NOTE: The RX data buffer must be clear or this will continue
	//to generate interrupts. Pg 157.
	UCSR0B |= (1<<RXCIE0);
	
	//Enables global interrupts.
	sei();
	
	//Enable receiver and transmitter
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	
	//Set frame format: 8data, 1 stop bit
	UCSR0C |= (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);   // 8bit data format
	
}


void USART_Transmit( unsigned char data )
{
	//Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	//Put data into buffer, sends the data
	UDR0 = data;
}

//This functions uses a character pointer (the "*" before the StringOfCharacters
//makes this a pointer) to retrieve a letter from a temporary character array (string)
//we made by passing the function "ALABTU!"
void Serial_Print(char *StringOfCharacters){
	
	//Let's do this until we see a zero instead of a letter.
	while(*StringOfCharacters > 0){
		
		//This function actually sends each character, one by one.
		//After a character is sent, we increment the pointer (++).
		USART_Transmit(*StringOfCharacters++);
	}
	//Let's signal the end of a string.
	LF();  //Sends a line-feed character.
	CR();  //Sends a carriage-return character.
}

void LF(){USART_Transmit(0x0A);}  //Function for sending line-feed character.
void CR(){USART_Transmit(0x0D);}  //Function for sending carriage-return character.
	
ISR(USART0_RX_vect){
	//This fires whenever a byte is received.
	PORTA ^= (1 << PINA6);
	ReceivedByte = UDR0;
}