
#include "../../augment/random.h"
#include "policyParameters.h"


#include "mergePolicy.h"



void MergePolicy::restart() {
	countSendMergeSyncs = 0;
}



bool MergePolicy::shouldScheduleMerge() {
	/*
	 * Collision avoidance: choose randomly whether to schedule mergeSlot.
	 *
	 * Contention:
	 * - other fishers who caught the same merged clique
	 * - the master of the merged clique
	 *
	 * This implementation may remain in role isMerger a long time (a long random sequence of coin flips yielding heads.)
	 * The intent is to minimize scheduled tasks: we don't schedule mergeSlot until the start of period it occurs in.
	 *
	 * Alternative: choose once, a random time in the future, and schedule a mergeSlot in some distant sync period.
	 * But then the task is scheduled for a long time and many sync periods may happen meanwhile.
	 */
	//assert(isActive);	// require
	return randBool();	// Fair coin flip
}




int MergePolicy::countSendMergeSyncs;

/*
 * Called every send MergeSync.
 * Return true if have xmitted finite count of MergeSync,
 * i.e. whether with high probability most mergees have heard a sync
 * and it is time to quit role Merger.
 *
 * Even if some mergees have not heard a MergeSync (and merged)
 * we can fish them again.
 */
bool MergePolicy::checkCompletionOfMergerRole() {
	//assert(isActive());	// require
	countSendMergeSyncs++;

	bool result = false;
	if ( countSendMergeSyncs > Policy::CountSyncsPerMerger ){
		result = true;
		// active = false;
	}
	return result;
}
