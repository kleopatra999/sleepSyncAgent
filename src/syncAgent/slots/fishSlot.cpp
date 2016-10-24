/*
 * THE fishSlot of my schedule.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>

#include "../globals.h"
#include "fishSlot.h"


bool FishSlot::dispatchMsgReceived(SyncMessage* msg){
	bool foundDesiredMessage;

	switch(msg->type) {
	case MasterSync:
		/*
		 * Intended catch: another clique's sync slot.
		 */
		doMasterSyncMsg(msg);
		// Stop listening: self can't handle more than one, or slot is busy with another merge
		foundDesiredMessage = true;
		break;
	case MergeSync:
		/*
		 * Unintended catch: Other (master or slave)
		 * is already xmitting into this time thinking it is SyncSlot of some third clique.
		 * Ignore except to stop fishing this slot.
		 */
		log("MergeSync in fish slot\n");
		foundDesiredMessage = true;
		break;
	case AbandonMastership:
		/*
		 * Unintended catch: Another clique's master is abandoning (exhausted power)
		 * For now ignore. Should catch clique again later, after another member assumes mastership.
		 */
		break;
	case Work:
		/*
		 * Unintended catch: Another clique's work slot.
		 * For now ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
		 * Alternative: since work slot follows syncSlot, could calculate syncSlot of catch, and merge it.
		 * Alternative: if work can be done when out of sync, do work.
		 */
		break;
	}

	return foundDesiredMessage;
}


void FishSlot::perform() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot
	assert(!radio->isPowerOn());
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisFishSlotStart());


	start();
	syncAgent.dispatchMsgUntil(
			dispatchMsgReceived,
			clique.schedule.deltaToThisFishSlotEnd);
	assert(radio->isDisabledState());
	end();
}


void FishSlot::start() {
	radio->powerOnAndConfigure();
	radio->configureXmitPower(8);
	sleeper.clearReasonForWake();
	radio->receiveStatic();		// DYNAMIC receiveBuffer, Radio::MaxMsgLength);
	// assert can receive an event that wakes imminently: race to sleep
}


void FishSlot::end(){
	/*
	 * Conditions:
	 * (no sync msg was heard and receiver still on)
	 * OR (heard a sync msg and (adjusted my schedule OR changed role to Merger))
	 *
	 * In case we adjusted my schedule, now is near its beginning.
	 * The next scheduled sync slot will be almost one full period from now.
	 *
	 * In case we adjusted changed role to Merger,
	 * first mergeSlot will be after next syncSlot.
	 */
	// radio might be off already
	radio->powerOff();
}


void FishSlot::doMasterSyncMsg(SyncMessage* msg){
	syncAgent.toMergerRole(msg);
	assert(syncAgent.role.isMerger());
	/*
	 * assert (schedule changed AND self is merging my former clique)
	 * OR (schedule unchanged AND self is merging other clique)
	 */
	// assert my schedule might have changed
}



/*
 * FUTURE act on it even though we are out of sync
void FishSlot::doWorkMsg(SyncMessage msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}
*/

