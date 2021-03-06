
#include <cassert>
#include <inttypes.h>
#include <cstdlib>	// rand() etc

#include "random.h"	// Our random.h, not from libstdc++


// FUTURE class and namespace to hide

/*
 * Utility routines, assuming platform only provides rand()
 *
 * Notes:
 *
 * Std C rand() yields value in range [0, RAND_MAX],
 * Some sources say RAND_MAX >= 32767,
 * i.e. result fits in, but does not uniformly fill uint_16!!!
 * (!!! The shell command rand() yields random bytes i.e. uint8_t.)
 *
 * % is remainder operator.
 * With signed operands, % may yield negative result on some implementations.
 * Here we explicitly use unsigned operands.
 */

uint16_t randUnsignedInt16(uint16_t min, uint16_t max) {
	/*
	 * Typical implementation with slight flaws in uniform distribution:
	 * Unless max-min divides int without a remainder, binning means some bins receive more
	 * values, i.e. non-uniform distribution.
	 */
	assert( max >= min);
	assert((max - min) < RAND_MAX);	// else not uniformly distributed
	int intResult = rand() % ( max + 1 - min) + min;

	// Convert, i.e. take lower two LSB
	assert(intResult < UINT16_MAX);	// No loss of data
	uint16_t result = (uint16_t) intResult;
	assert (result >= min && result <= max);	// ensure result as specified
	return result;
}

/*
 * Random flip of a fair coin.
 * Depends on rand()
 */
bool randBool() {
	unsigned int result = rand() % 2;
	assert( result == 0 || result == 1);
	return result;	// coerce to bool
}
