
typedef unsigned char uint8_t ;


#include <iostream>
#include <boost/assert.hpp>
#include "inputfilter.h"


static const int sampleSize = 14;

class DummyProvider {
	int counter;
	const uint8_t* samples;
public:
	DummyProvider(const uint8_t* sampleData) : counter(0), samples(sampleData) {};

	uint8_t read() {
		if ( counter >= sampleSize )
			counter = 0;
		return samples[counter++];
	}
};

static const uint8_t allBelow32[sampleSize] = { 5, 6, 5, 0, 8, 10, 22, 20, 17, 0, 1, 0 , 30 , 31 };
class AllBelow32Provider : public DummyProvider {
public:
	AllBelow32Provider() : DummyProvider( allBelow32 ) {};
};

static const uint8_t unstable[sampleSize] = { 5, 6, 105, 0, 208, 10, 22, 20, 17, 0, 1, 80 , 30 , 31 };
class UnstableProvider : public DummyProvider {
public:
	UnstableProvider() : DummyProvider( unstable ) {};
};

static const uint8_t stepTo32[sampleSize] = { 5, 6, 33, 34, 33, 40, 50, 60, 52, 40, 45, 34 , 34 , 34 };
class StepTo32Provider : public DummyProvider {
public:
	StepTo32Provider() : DummyProvider( stepTo32 ) {};
};



int main()
{
	InputFilter<AllBelow32Provider> inputFilter1;
	for ( int i = 0; i < sampleSize; ++i) {
		int value = inputFilter1.run();
		if ( i < InputFilter<DummyProvider>::sampleStabilityCount )
			assert (0 == value );
		else
			assert (1 == value );
	}


	InputFilter<UnstableProvider> inputFilter2;
	for ( int i = 0; i < sampleSize; ++i) {
		int value = inputFilter2.run();
		assert (0 == value );
	}


	InputFilter<StepTo32Provider> inputFilter3;
	for ( int i = 0; i < sampleSize; ++i) {
		int value = inputFilter3.run();
		if ( i < 12 )
			assert (0 == value );
		else
			assert (2 == value );	}
}
