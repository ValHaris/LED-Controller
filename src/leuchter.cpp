/*  Phaser MK3 main program
    Copyright 2009 by M. Bickel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/
*/


#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "adc.h"
#include "output.h"
#include "inputfilter.h"

Output output;



struct Timer {
    typedef uint16_t Type;
    static inline Type getTime() {
        return TCNT1;
    }
    static inline void setTime( Type time ) {
        TCNT1 = time;
    }
};



void runLights()
{
    static uint8_t light = 255;

    static const uint16_t lightPattern[9] = { 0, 24, 36, 66, 129, 129<<8,66<<8,36<<8,24<<8 };

    static Timer::Type lastTime;

    if ( light == 255 ) {
        lastTime = TCNT1;
        light = 0;
    }

    if ( Timer::getTime() - lastTime > 210 ) {
        ++light;
        if ( light > 8 )
            light = 0;
        lastTime = Timer::getTime();
    }

    for ( uint8_t i = 0; i < Output::ledNum; ++i )
        output.setLED( i, !(lightPattern[light] & (1<<i) ));
}


class Debug {
public:
	Debug() {
		DDRA |= 1<<PA1;
		setValue(0);
	}
	void setValue( bool value ) {
		if ( value )
			PORTA &= ~(1<<PA1);
		else
			PORTA |= 1 << PA1;
	}
} debug;

typedef InputFilter<SingleChannelADConverter<0,2> > Input;

Input input;


class PWM0 {
public:
    PWM0() {
        TCCR0A = (TCCR0A & ~7) + 3; // Fast PWM Mode 	, counting to 0xff

        // Clear OC0A on Compare Match
        // Set OC0A at BOTTOM (non-inverting mode)
        TCCR0A |= 1 << COM0A1;
        TCCR0A &= ~(1 << COM0A0);

        TCCR0A |= 1 << COM0B1;
        TCCR0A &= ~(1 << COM0B0);


        // clock prescaled by 8
        TCCR0B &= ~((1<< CS02) | (1<<CS00));
        TCCR0B |= (1<<CS01);

        DDRB |= (1 << PB2); // output on OC0A
        DDRA |= (1 << PA7); // .. and OC0B
    }

    void setValue0A( uint8_t value) {
        OCR0A = value;
    }

    void setValue0B( uint8_t value ) {
        OCR0B = value;
    }
};

class PWM1 {
public:
    PWM1() {
        // Fast PWM, 8-bit
        TCCR1A |= (1 << WGM10);
        TCCR1A &= ~(1 << WGM11);
        TCCR1B |= 1 << WGM12;
        TCCR1B &= ~(1 << WGM13);

        // Clear OC0A on Compare Match
        // Set OC0A at BOTTOM (non-inverting mode)
        TCCR1A |= 1 << COM1A1;
        TCCR1A &= ~(1 << COM1A0);

        TCCR1A |= 1 << COM1B1;
        TCCR1A &= ~(1 << COM1B0);


        // clock prescaled by 8
        TCCR1B &= ~((1<< CS12) | (1<<CS10));
        TCCR1B |= (1<<CS11);

        DDRA |= (1 << PA6); // output on OC1A
        DDRA |= (1 << PA5); // .. and OC1B
    }

    void setValue1A( uint8_t value) {
        OCR1A = value;
    }

    void setValue1B( uint8_t value ) {
        OCR1B = value;
    }
};

static const uint8_t powersteps[Input::steps] = {
		0 * 0xff / (Input::steps-1),
		1 * 0xff / (Input::steps-1),
		2 * 0xff / (Input::steps-1),
		3 * 0xff / (Input::steps-1),
		4 * 0xff / (Input::steps-1),
		5 * 0xff / (Input::steps-1),
		6 * 0xff / (Input::steps-1),
		7 * 0xff / (Input::steps-1)
};

int main()
{
    PWM0 pulser;
    PWM1 pulser2;
    pulser.setValue0A(0);
    pulser.setValue0B(0);

    pulser2.setValue1A(0);
    pulser2.setValue1B(0);


    while(1) {

    	uint8_t value = input.run();
    	if ( value != 0 ) {
    		uint8_t power = powersteps[value-1];
    		pulser.setValue0A( power );
    		pulser.setValue0B(0xff - power );

    	    pulser2.setValue1A( power );
    	    pulser2.setValue1B(0xff - power );
    	    if ( value == 8 )
    	    	debug.setValue(1);
    	    else
    	    	debug.setValue(0);
    	}
    }

    return 0;
}
