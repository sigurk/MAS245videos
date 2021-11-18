/*
 * prosjekt1oppgave3.cpp
 *
 * Created: 14.10.2021 12:37:24
 * Author : S21H
 */ 

#define F_CPU 20000000UL // Forteller delay hvor rask mikrochip kjører, ATmega 168 = 20 mhz

#include <avr/io.h>
#include <util/delay.h>


void initDTimer(){
	DDRD |= 0x0;										// Init DDRD, god praksis
	DDRD |= 1 << PD6;									// Init PD6
	TCCR0A |= 1 << WGM00 | 1 << WGM01 | 1 << COM0A1;    // Opp til 0xFF-nullstilles...
	TCCR0B |= 1 << CS00;								// Prescaler 
}

void SetPWMoutput(int duty)
{
	OCR0A = duty;
}

int main(void)
{
	initDTimer();
	int brightness = 0;									

    while (1) 
    {
		for(brightness = 0; brightness < 40; brightness++)
		{
			SetPWMoutput(brightness);
			_delay_ms(1);
		}
		for(brightness = 40; brightness > 0; brightness--)
		{
			SetPWMoutput(brightness);
			_delay_ms(1);
		}
		

    }
}

