class ADConverter {
public:
    ADConverter()  {

        // die Versorgungsspannung AVcc als Refernz wählen:
        ADMUX = 7;

        // Bit ADFR ("free running") in ADCSRA steht beim Einschalten
        // schon auf 0, also single conversion
        ADCSRA |= (1<<ADPS2) | (1<<ADPS0);     // Frequenzvorteiler 128
        ADCSRA &= ~(1<<ADPS1);
        ADCSRA |= (1<<ADEN);                  // ADC aktivieren

        ADCSRB |= (1<<ADLAR);

        /* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
           also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */

        read(7);
    }

    /* ADC Einzelmessung */
    static uint8_t read( uint8_t channel )
    {
        // Kanal waehlen, ohne andere Bits zu beeinflußen
        ADMUX = (ADMUX & ~(0x3F)) | (channel & 0x3F);
        ADCSRA |= (1<<ADSC);            // eine Wandlung "single conversion"
        while (ADCSRA & (1<<ADSC) ) {   // auf Abschluss der Konvertierung warten
        }
        return ADCH;                    // ADC auslesen und zurückgeben
    }

    /* ADC Mehrfachmessung mit Mittelwertbbildung */
    /* beachte: Wertebereich der Summenvariablen */
    static uint8_t read_Avg( uint8_t channel, uint8_t nsamplesPowerOfTwo )
    {
        uint16_t sum = 0;

        uint8_t cnt = 1 << nsamplesPowerOfTwo;

        for (uint8_t i = 0; i < cnt; ++i ) {
            sum += read( channel );
        }

        return (uint8_t)( sum >> nsamplesPowerOfTwo );
    }
};

template <int channel, int averageSamples>
class SingleChannelADConverter {
private:
	ADConverter converter;
public:
	uint8_t read() {
		return ADConverter::read_Avg(channel,averageSamples);
	};
};
