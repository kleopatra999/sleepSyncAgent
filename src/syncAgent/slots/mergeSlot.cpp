
#include <cassert>

#include "../globals.h"
#include "mergeSlot.h"



namespace {

/*
 * Partial application:
 * Convert function of one arg into function of no args.
 *
 * A method that whenever called, returns time remaining until time to perform merge.
 */
DeltaTime timeoutUntilMerge() {
	// TODO minor adjustment to account for ramp up of radio
	return clique.schedule.deltaToThisMergeStart(
			syncAgent.cliqueMerger.getOffsetToMergee());
}

} // namespace



// static data member
MergePolicy MergeSlot::mergePolicy;

/*
 * Sleep all normally sleeping slots until time to xmit MergeSynce into mergee clique's SyncSlot.
 * !!! The time to xmit is not aligned with this schedule's slots, but with middle of mergee's SyncSlot.
 */
void MergeSlot::perform() {
	assert(!radio->isPowerOn());
	assert(role.isMerger());
	// Hard sleep without listening.
	syncSleeper.sleepUntilTimeout(timeoutUntilMerge);

	network.preamble();

	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	network.prepareToTransmitOrReceive();
	logLongLong(clique.schedule.nowTime()); log(":mergeSync");
	syncSender.sendMergeSync();
	network.shutdown();

	if (mergePolicy.checkCompletionOfMergerRole()){
		mergePolicy.restart();
		syncAgent.toFisherRole();
		// assert next SyncPeriod will schedule FishSlot
		assert(!role.isMerger());
		assert(role.isFisher());
	}
	// else continue in role Merger

	network.postlude();

	assert(!radio->isPowerOn());
}

