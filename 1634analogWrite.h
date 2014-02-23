#ifndef analogWrite1634
#define analogWrite1634

#include <avr/io.h>
#include <util/delay.h>

void analogWrite(int PWM_PinSelect, int duty);

// initialize PWM
void pwm_init()
{
	//Define PWM pins.
	#define PWM_PC0 1
	#define PWM_PA5 2
	#define PWM_PA6 3
	#define PWM_PB3 4

	//This is the first PWM register, TCNT0.  It is 8 bit.  Both PIN PA5 and PA6 are set to clear on compare,
	//then set at bottom; this makes them non-inverting.  The WGM bits are set to for "Fast PWM MODE"
	//and this clears at the top, "0x00FF."
	TCCR0A = 0b00000011; // WORKS FOR OC0A, OC0B
	TCCR0B = 0b00000001; // WORKS FOR OC0A, OC0B

	//This is the second PWM register;TCNT1.  It is 8 bit.  Both PIN PB3 and PC0 are set to clear on compare,
	//then set at bottom; this makes them non-inverting.  The WGM bits are set to for "Fast PWM MODE"
	//and this clears at the top, "0x00FF."
	TCCR1A = 0b00000001;  //WORKS FOR OC1A, OC1B
	TCCR1B = 0b00001001;  //WORKS FOR OC1A, OC1B
	
	//This sets the PWM pins as outputs.
	DDRB |= (1<<PINB3);
	DDRA |= (1<<PINA5);
	DDRA |= (1<<PINA6);
	DDRC |= (1<<PINC0);
	
}

void analogWrite(int PWM_PinSelect, int duty){
	
	//Make sure we were passed a number in-range.
	if (duty > 255) duty = 255;
	if (duty < 1) duty = 0;
	
	//Sets PWM for PC0
	if (PWM_PinSelect == 1){
		if (duty > 0){
			TCCR0A |= (1<<COM0A1);
			OCR0A = duty;
		}
		else {
			TCCR0A ^= (1<<COM0A1);
		}
	}
	
	//Sets PWM for PA5
	if (PWM_PinSelect == 2){
		if (duty > 0){
			TCCR0A |= (1<<COM0B1);
			OCR0B = duty;
		}
		else {
			TCCR0A ^= (1<<COM0B1);
		}
	}
	
	//Sets PWM for PA6
	if (PWM_PinSelect == 3){
		if (duty > 0){
			TCCR1A |= (1<<COM1B1);
			OCR1B = duty;
		}
		else {
			TCCR1A ^= (1<<COM1B1);
		}
	}
	
	//Sets PWM for PB3
	if (PWM_PinSelect == 4){
		if (duty > 0){
			TCCR1A |= (1<<COM1A1);
			OCR1A = duty;
		}
		else {
			TCCR1A	 ^= (1<<COM1A1);
		}
	}
	
}

#endif