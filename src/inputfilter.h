template<class InputProvider>
class InputFilter {

    uint8_t lastValue;
    uint8_t lastValueCount;

    InputProvider inputProvider;

public:
    static const int steps = 8; // sollte eine 2er Potenz sein
    static const int sampleStabilityCount = 10;

    InputFilter() : lastValue(0xff), lastValueCount(0)
    {

    }

    /* converts the raw value into a step */
    inline uint8_t transform(uint8_t value) {
    	return (value / (256/steps)) + 1;
    }



    /* Returns 1..8 for the detected value, or 0 if there has been no stable input */
    uint8_t run() {
    	uint8_t value = transform( inputProvider.read() );

    	if ( lastValue == value ) {
    		if ( lastValueCount >= sampleStabilityCount ) {
    			return value;
    		} else {
    			++lastValueCount;
    			return 0;
    		}
    	} else {
    		lastValueCount = 1;
    		lastValue = value;
    		return 0;
    	}
    }

};
