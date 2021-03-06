#pragma once

#include <cassert>
#include "../types.h"	// ScheduleCount


// FUTURE, resettable and a policy that fishes outward in both directions from sync slot.
/*
 * Generator of sequence of ordinal of normally sleeping slot to fish in.
 *
 * Abstract base class API:
 * - next()
 * - reset()
 *
 * Subclasses:
 *
 * SimpleFishPolicy
 * - circular, ascending
 * - in sequential order
 * - reset() has no effect
 *
 * SyncRecoveryFishPolicy:
 * - expands outward from SyncPoint
 * - alternates direction, ascending and descending
 *
 * next() returns ordinal, i.e. not a zero-based index.
 */


/*
 * Yields: 3,4,...,CountSlots,2,3,4...
 * i.e. after a reset it does not start at the first sleeping slot
 */
class SimpleFishPolicy {
public:
	ScheduleCount nextFishSlotOrdinal();
	void reset() {}	// Does nothing, generator continues as before
};


/*
 * After reset(), fans outward from current SyncSlot.
 * Attempts to quickly find a drifted Master.
 *
 * All units may do the same thing.
 * It does not cause contention, because fishing is receiving.
 * However, when all units recover the same Master,
 * then there could be contention to MergeSync.
 * So elsewhere the design should avoid that contention.
 *
 * In this example, the numbers are the ordinals of the slots,
 * where there are 8 slots and slot 1 is the SyncSlot.
 * Yields 2,8,3,7,4,6,5,5,6,4,7,3,8,2,2,8,3,...
 *
 * Note there are two places where the same slot is fished consecutively.
 *
 * !!! Note it is vital to start at the first sleeping slot past and before the SyncSlot.
 * That is where a drifted master is most likely to be.
 */
class SyncRecoveryFishPolicy {
public:
	ScheduleCount nextFishSlotOrdinal();
	void reset();
};
