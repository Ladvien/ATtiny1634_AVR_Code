// UART Example on the ATtiny1634 using UART0.
// C. Thomas Brittain
// letsmakerobots.com
#define F_CPU 8000000UL		//AVR clock frequency in Hz, used by util/delay.h
#include <avr/io.h>			//Holds Pin and Port defines.
#include <util/delay.h>		//Needed for delay.
#include <avr/sleep.h>		//Needed for sleep mode.
#include "1634_UART.h"

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
	USART_init0(19200);
	USART_init1(19200);
	
	//Set Sleep
	set_sleep_mode(SLEEP_MODE_IDLE);
	
	//Forever loop.
	while(1){
		//ReceivedData = "ASDASDAS";
		sleep_mode();
		
		//USART0
		if (ReceivedData0[(ReceivedDataIndex0)-1]==0x2E){
			//Function to print the RX buffer
			Serial_Print1(ReceivedData0);
			//Let's signal the end of a string.
			LF1();CR1(); //Ending characters.
			//After we used the data from buffer, clear it.
			clearBuffer0();
			//Reset the RX flag.
			rxFlag0 = 0;
		}

		//USART1
		if (ReceivedData1[(ReceivedDataIndex1)-1]==0x2E){
			//Function to print the RX buffer
			Serial_Print0(ReceivedData1);
			//Let's signal the end of a string.
			LF0();CR0(); //Ending characters.
			//After we used the data from buffer, clear it.
			clearBuffer1();
			//Reset the RX flag.
			rxFlag1 = 0;
		}		
		
		//_delay_ms(500);
	}
}

