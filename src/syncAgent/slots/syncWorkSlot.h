
#pragma once

#include "syncWorkSlot.h"

/*
 * Combined work/sync slot.
 * Sync period has one sync slot and no separate work slot.
 * WorkSync message carries sync.
 *
 * Each Clique is on a Schedule that includes a sync slot.
 * See general notes in Schedule.
 *
 *Cases:
 * a) A clique's Master xmits sync FROM its sync slot.
 *
 * b) A Merger xmits sync FROM a sleeping slot INTO another clique's sync slot.
 *
 * c) One clique's sleeping slot may coincidentally be near in time to another clique's sleeping slot, or drift into each other.
 *
 * d) A clique Master or Slave may xmit work from this slot, carrying sync info
 *
 * Cases b-d have contention (two syncs in same interval of a sync slot):
 * - b: merging: a Merger of a better clique may be xmitting sync FROM its sleeping slot to merge this clique into an offset syncSlot
 * - c: two cliques may unwittingly be in sync (two Masters xmitting in same interval)
 * - d: a Master xmitting sync and a Slave xmitting work
 *
 * Slot is divided into three subslots:
 * - listen
 * - xmit
 * - listen
 * Any xmit is in the center of the slot (centered in the middle subslot.)
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 */


class SyncWorkSlot {
private:
	static bool doListenHalfSyncWorkSlot(OSTime (*timeoutFunc)());

	/*
	 * Three behaviours of slot:
	 * - Master: listen and send in middle
	 * - Slave listening
	 * - Master or Slave, listening and conveying work in middle
	 * 
	 * Not local because they refer to superclass dispatcher i.e. this
	 */
	static void doSendingWorkSyncWorkSlot();
	static void doMasterSyncWorkSlot();
	static void doSlaveSyncWorkSlot();
	
	
public:
	static void perform();
	static bool dispatchMsgReceived(SyncMessage* msg);	// Same code as for SyncWorkSlot
	static bool doMasterSyncMsg(SyncMessage* msg);
	static bool doMergeSyncMsg(SyncMessage* msg);
	static bool doAbandonMastershipMsg(SyncMessage* msg);
	static bool doWorkMsg(SyncMessage* msg);
};
