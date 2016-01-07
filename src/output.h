/*  Phaser MK3 output handler 
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



class Output {
public:
	static const int ledNum = 3;
	uint8_t brightness[ledNum];

private:

	void configure() {
		// configure LED ports for output
		for ( uint8_t i = 0; i < ledNum; ++i )
			DDRA |= 1 << i;
	}
	uint8_t counter;

public:

	Output() : counter(0)
	{
		configure();
	}

	void output ( uint8_t value ) {
		for ( uint8_t i = 0; i < ledNum; ++i)
			if ( value & (1<<i))
				brightness[i] = 0xff;
			else
				brightness[i] = 0;
	}

	void setLED( uint8_t num, uint8_t bright )
	{
		brightness[num] = bright;
	}

	uint8_t loop()
	{
		counter++;

		uint8_t out = 0;

		for ( uint8_t l = 0; l < ledNum; ++l) {
			if ( counter > brightness[l] )
				out |= 1 << l;
		}
		PORTA = out;
		return counter;
	}
} ;
