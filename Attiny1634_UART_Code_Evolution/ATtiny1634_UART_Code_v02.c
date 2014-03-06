//
// UART Example on the ATtiny1634 using UART0.
// C. Thomas Brittain
// letsmakerobots.com

#define F_CPU 8000000UL    // AVR clock frequency in Hz, used by util/delay.h
#include <avr/io.h>
#include <util/delay.h>

//Preprocessing of functions.  This allows us to intialize functions
//without having to put them before the main.
void uart_init ();
void USART_Transmit( unsigned char data );
void Serial_Print(char *StringOfCharacters);
	
	
// Define some macros
#define UBBR 25 // 9600, .02%

// Main
int main()
{
	//Initialize the serial connection.
	uart_init();
	
	//Forever loop.
	while(1){
		//Call the Serial_Print function.
		Serial_Print("ALABTU!");		
	}
}

// function to initialize UART
void uart_init ()
{
	//Set baud rate
	UBRR0H = (unsigned char)(UBBR>>8);
	UBRR0L = (unsigned char)UBBR;
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);   // 8bit data format
}

void USART_Transmit( unsigned char data )
{
	//Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	//Put data into buffer, sends the data
	UDR0 = data;
}

//This functions uses a character pointer (the "*" before the StringOfCharacters
//makes this a pointer) to retrieve a letter from the temporary character buffer
//we made by passing the function "ALABTU!" 
void Serial_Print(char *StringOfCharacters){
	
	//Let's do this until we see a zero instead of a letter.
	while(*StringOfCharacters > 0){
	
		//This function actually sends each character, one by one.
		//After a character is sent, we increment the pointer (++).
		USART_Transmit(*StringOfCharacters++);
	}
}