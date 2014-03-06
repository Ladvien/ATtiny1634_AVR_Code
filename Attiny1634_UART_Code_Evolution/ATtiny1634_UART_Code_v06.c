/ UART Example on the ATtiny1634 using UART0.
// C. Thomas Brittain
// letsmakerobots.com
#define F_CPU 8000000UL		//AVR clock frequency in Hz, used by util/delay.h
#include <avr/io.h>			//Holds Pin and Port defines.
#include <util/delay.h>		//Needed for delay.
#include <avr/interrupt.h>  //Add the interrupt library; int. used for RX.
#include <avr/sleep.h>		//Needed for sleep mode.


//Buffers for UART0 and UART1
//USART0
char ReceivedData0[32];	//Character array for Rx data.
int ReceivedDataIndex0;	//Character array index.
int rxFlag0;			//Boolean flag to show character has be retrieved from RX.

//USART1
char ReceivedData1[32];	//Character array for Rx data.
int ReceivedDataIndex1;	//Character array index.
int rxFlag1;			//Boolean flag to show character has be retrieved from RX.

//Preprocessing of functions.  This allows us to initialize functions
//without having to put them before the main.
void uart_init0(int BUADRATE);
void USART_Transmit0( unsigned char data);
void Serial_Print0(char *StringOfCharacters);
void clearBuffer0();

void uart_init1(int BUADRATE);
void USART_Transmit1( unsigned char data);
void Serial_Print1(char *StringOfCharacters);
void clearBuffer1();

//EOT characters.
void LF0();
void CR0();

//EOT characters.
void LF1();
void CR1();


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
	uart_init0(19200);
	uart_init1(19200);
	
	//Set Sleep
	set_sleep_mode(SLEEP_MODE_IDLE);
	
	//Forever loop.
	while(1){
		//ReceivedData = "ASDASDAS";
		sleep_mode();
		
		//USART0
		if (ReceivedData0[(ReceivedDataIndex0)-1]==0x2E){
			//Function to print the RX buffer
			Serial_Print0(ReceivedData0);
			//Let's signal the end of a string.
			LF0();CR0(); //Ending characters.
			//After we used the data from buffer, clear it.
			clearBuffer0();
			//Reset the RX flag.
			rxFlag0 = 0;
		}

		//USART1
		if (ReceivedData1[(ReceivedDataIndex1)-1]==0x2E){
			//Function to print the RX buffer
			Serial_Print1(ReceivedData1);
			//Let's signal the end of a string.
			LF1();CR1(); //Ending characters.
			//After we used the data from buffer, clear it.
			clearBuffer1();
			//Reset the RX flag.
			rxFlag1 = 0;
		}		
		
		//_delay_ms(500);
	}
}

// function to initialize UART0
void uart_init0(int Desired_Baudrate)
{
	//Only set baud rate once.  If baud is changed serial data is corrupted.
	#ifndef UBBR
		//Set the baud rate dynamically, based on current microcontroller
		//speed and the desired baud rate given by the user.
		#define UBBR ((F_CPU)/(Desired_Baudrate*16UL)-1)
	#endif
		
	//Set baud rate.
	UBRR1H = (unsigned char)(UBBR>>8);
	UBRR1L = (unsigned char)UBBR;
	
	//Enables the RX interrupt.
	//NOTE: The RX data buffer must be clear or this will continue
	//to generate interrupts. Pg 157.
	UCSR1B |= (1<<RXCIE1);
	
	//Enable receiver and transmitter
	UCSR1B |= (1<<RXEN1)|(1<<TXEN1);
	
	//Set frame format: 8data, 1 stop bit
	UCSR1C |= (1<<UCSZ00)|(1<<UCSZ01);   // 8bit data format
	
	//Enables global interrupts.
	sei();	
}

// Function to initialize UART1
void uart_init1(int Desired_Baudrate)
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
	
	//Enables global interrupts.
	sei();
}

//USART0
void USART_Transmit0( unsigned char data )
{
	//We have to disable RX interrupts.  If we have
	//an interrupt firing at the same time we are
	//trying to transmit we'll lose some data.
	UCSR0B ^= ((1<<RXEN0)|(1<<RXCIE0));

	//Wait for empty transmit buffer
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	//Put data into buffer, sends the data
	UDR0 = data;

	//Re-enable RX interrupts.
	UCSR0B ^= ((1<<RXEN0)|(1<<RXCIE0));

}

//USART1
void USART_Transmit1( unsigned char data )
{
	
	//We have to disable RX interrupts.  If we have
	//an interrupt firing at the same time we are
	//trying to transmit we'll lose some data.
	UCSR1B ^= ((1<<RXEN1)|(1<<RXCIE1));

	//Wait for empty transmit buffer
	while ( !( UCSR1A & (1<<UDRE1)) );
	
	//Put data into buffer, sends the data
	UDR1 = data;

	//Re-enable RX interrupts.
	UCSR1B ^= ((1<<RXEN1)|(1<<RXCIE1));

}

//This functions uses a character pointer (the "*" before the StringOfCharacters
//makes this a pointer) to retrieve a letter from a temporary character array (string)
//we made by passing the function "ALABTU!"

//USART0
void Serial_Print0(char *StringOfCharacters){
	UCSR0B ^= ((1<<RXEN0)|(1<<RXCIE0));
	
	//Let's do this until we see a zero instead of a letter.
	while(*StringOfCharacters > 0){
		
		//This function actually sends each character, one by one.
		//After a character is sent, we increment the pointer (++).
		USART_Transmit0(*StringOfCharacters++);
	}
	//Re-enable RX interrupts.
	UCSR0B ^= ((1<<RXEN0)|(1<<RXCIE0));
}

//USART1
void Serial_Print1(char *StringOfCharacters){
	UCSR1B ^= ((1<<RXEN1)|(1<<RXCIE1));
	
	//Let's do this until we see a zero instead of a letter.
	while(*StringOfCharacters > 0){
		
		//This function actually sends each character, one by one.
		//After a character is sent, we increment the pointer (++).
		USART_Transmit1(*StringOfCharacters++);
	}
	//Re-enable RX interrupts.
	UCSR1B ^= ((1<<RXEN1)|(1<<RXCIE1));
}

//USART0
void clearBuffer0(){
	//Ugh.  A very inefficient way to clear the buffer. :P
	ReceivedDataIndex0=0;
	for (unsigned int i = 0; i < 64;)
	{
		//We set the buffer to NULL, not 0.
		ReceivedData0[i] = 0x00;
		i++;
	}	
}

//USART1
void clearBuffer1(){
	//Ugh.  A very inefficient way to clear the buffer. :P
	ReceivedDataIndex1=0;
	for (unsigned int i = 0; i < 64;)
	{
		//We set the buffer to NULL, not 0.
		ReceivedData1[i] = 0x00;
		i++;
	}
}

void LF0(){USART_Transmit0(0x0A);}  //Function for sending line-feed character.
void CR0(){USART_Transmit0(0x0D);}  //Function for sending carriage-return character.

void LF1(){USART_Transmit1(0x0A);}  //Function for sending line-feed character.
void CR1(){USART_Transmit1(0x0D);}  //Function for sending carriage-return character.
ISR(USART0_RX_vect){	
	//RX0 interrupt
	
	//Show we have received a character.
	rxFlag0 = 1;
	
	//Load the character into the poor man's buffer.
	//The buffer works based on a end-of-transmission character (EOTC)
	//sent a the end of a string.  The buffer stops at 63 instead of 64
	//to always give room for this EOTC.  In our case, "."
	if (ReceivedDataIndex0 < 63){
		//Actually pull the character from the RX register.
		ReceivedData0[ReceivedDataIndex0] = UDR0;
		//Increment RX buffer index.
		ReceivedDataIndex0++;
	}
	else {
		//If the buffer is greater than 63, reset the buffer.
		ReceivedDataIndex0=0;
		clearBuffer0();		
	}
}

ISR(USART1_RX_vect){
	//RX1 interrupt
	PORTA ^= (1 << PINA6);
	//Show we have received a character.
	rxFlag1 = 1;
	
	if (ReceivedDataIndex1 < 63){
		//Actually pull the character from the RX register.
		ReceivedData1[ReceivedDataIndex1] = UDR1;
		//Increment RX buffer index.
		ReceivedDataIndex1++;
	}
	else {
		//If the buffer is greater than 63, reset the buffer.
		ReceivedDataIndex1=0;
		clearBuffer1();
	}
}