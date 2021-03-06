#pragma once

#include "../modules/message.h"

/*
 * Superclass API for a slot.
 * Subclasses:
 *    SyncWorkSlot
 *    FishSlot
 *
 * MergeSlot is different (not a Slot)
 * WorkSlot is obsolete.
 *
 */
class Slot {

	/*
	 * All slots use radio
	 */



public:
	/*
	 * Default behaviour: return false, don't do anything with msg, and continue looking for messages
	 *
	 * Not pure virtual: subclasses must override for different behaviour.
	 */
	// virtual destructor to avoid warnings (with , but it causes link errors using --nodefaultlibs
	//virtual ~Slot() {};



	virtual bool doMasterSyncMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doMergeSyncMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doAbandonMastershipMsg(SyncMessage* msg) {(void) msg; return false;};
	virtual bool doWorkMsg(SyncMessage* msg) {(void) msg; return false;};
};
