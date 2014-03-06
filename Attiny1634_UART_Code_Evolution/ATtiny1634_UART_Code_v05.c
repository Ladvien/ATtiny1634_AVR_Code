// UART Example on the ATtiny1634 using UART0.
// C. Thomas Brittain
// letsmakerobots.com
#define F_CPU 8000000UL		//AVR clock frequency in Hz, used by util/delay.h
#include <avr/io.h>			//Holds Pin and Port defines.
#include <util/delay.h>		//Needed for delay.
#include <avr/interrupt.h>  //Add the interrupt library; int. used for RX.
#include <avr/sleep.h>		//Needed for sleep mode.

char ReceivedData[32];	//Character array for Rx data.
int ReceivedDataIndex;	//Character array index.
int rxFlag;				//Boolean flag to show character has be retrieved from RX.

//Preprocessing of functions.  This allows us to initialize functions
//without having to put them before the main.
void uart_init (int BUADRATE);
void USART_Transmit( unsigned char data);
void Serial_Print(char *StringOfCharacters);
void LF();
void CR();
void clearBuffer();


// Main
int main()
{
	//Setup received data LED.
	DDRA |= (1 << PINA6);
	
	//Light LED on PA6 to show the chip has reboot.
	PORTA ^= (1 << PINA6);
	_delay_ms(500);
	PORTA ^= (1 << PINA6);
	
	//Initialize the serial connection and pass it a desired baud rate.
	uart_init(19200);
	
	//Set Sleep
	set_sleep_mode(SLEEP_MODE_IDLE);
	
	//Forever loop.
	while(1){
		//ReceivedData = "ASDASDAS";
		sleep_mode();
		
		if (ReceivedData[ReceivedDataIndex-1]==0x2E){
			//We have to disable RX interrupts.  If we have
			//an interrupt firing at the same time we are
			//trying to transmit we'll lose some data.
			UCSR0B ^= ((1<<RXEN0)|(1<<RXCIE0));
			
			//Function to print the RX buffer
			Serial_Print(ReceivedData);
			
			//Let's signal the end of a string.
			LF();CR(); //Ending characters.
			
			//After we used the data from buffer, clear it.
			clearBuffer();
			
			//Reset the RX flag.
			rxFlag = 0;
			
			//Re-enable RX interrupts.
			UCSR0B ^= ((1<<RXEN0)|(1<<RXCIE0));
		}
		PORTA ^= (1 << PINA6);
		//_delay_ms(500);
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
	
	//Enable receiver and transmitter
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	
	//Set frame format: 8data, 1 stop bit
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);   // 8bit data format
	//|(1<<USBS0)
	//Enables global interrupts.
	sei();	
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
}

void LF(){USART_Transmit(0x0A);}  //Function for sending line-feed character.
void CR(){USART_Transmit(0x0D);}  //Function for sending carriage-return character.

void clearBuffer(){
	//Ugh.  A very inefficient way to clear the buffer. :P
	ReceivedDataIndex=0;
	for (unsigned int i = 0; i < 64;)
	{
		//We set the buffer to NULL, not 0.
		ReceivedData[i] = 0x00;
		i++;
	}	
}

ISR(USART0_RX_vect){	
	//RX interrupt
	
	//Show we have received a character.
	rxFlag = 1;
	
	//Load the character into the poor man's buffer.
	//The buffer works based on a end-of-transmission character (EOTC)
	//sent a the end of a string.  The buffer stops at 63 instead of 64
	//to always give room for this EOTC.  In our case, "."
	if (ReceivedDataIndex < 63){
		//Actually pull the character from the RX register.
		ReceivedData[ReceivedDataIndex] = UDR0;
		//Increment RX buffer index.
		ReceivedDataIndex++;
	}
	else {
		//If the buffer is greater than 63, reset the buffer.
		ReceivedDataIndex=0;
		clearBuffer();		
	}
}