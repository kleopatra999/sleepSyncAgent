#include <cassert>
#include "longClock.h"



// static singleton data members
uint32_t LongClock::mostSignificantBits = 0;
uint32_t LongClock::previousOSClockTicks;	// least significant


void LongClock::reset(){
	mostSignificantBits = 0;	// aka epoch
	previousOSClockTicks = OSClockTicks();
	// assert nowTime() < max 32-bit int, but not zero.
}


/*
 * Called more often than OSClock wraps (rolls over.)
 */
LongTime LongClock::nowTime() {

	// Account for quiet wrapping of OSClock (we are not notified by OS.)
	OSTime currentOSClockTicks = OSClockTicks();
	if (currentOSClockTicks < previousOSClockTicks) {
		// OSClock wrapped
		mostSignificantBits++;	// Tick most significant bits
	}
	previousOSClockTicks = currentOSClockTicks;

	// Concatenate MSB and LSB.  Portable?
	LongTime result = mostSignificantBits;
	result = result << 32;	// Left shift, fill LSB with zero
	result = result | currentOSClockTicks;	// Bit-wise OR into LSB
	return result;
};


/*
 * LongTime math
 *
 * These return shorter ints as returned by OSClock and used by other OS methods
 */

// Can be called if you are not sure laterTime is after earlierTime
DeltaTime LongClock::clampedTimeDifference(LongTime laterTime, LongTime earlierTime) {
	// Returns positive time difference or zero
	DeltaTime result;
	if (earlierTime > laterTime) result = 0;
	else result = laterTime - earlierTime;	// !!! Coerce to 32-bit
	assert(result >= 0);
	// assert(result < 3 * PeriodDuration);	// Sanity, app does not schedule far in the future.
	return result;
}

// Requires futureTime less than 32-bit from now
DeltaTime LongClock::clampedTimeDifferenceFromNow(LongTime futureTime) {
	return clampedTimeDifference(futureTime, nowTime());	// !!! coerce to 32-bit
}