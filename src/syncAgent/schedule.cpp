
#include <cassert>
#include "schedule.h"
#include "time.h"

// OS
static void scheduleTask(void callback(), DeltaTime  ) {}


// static singleton data
static LongClock longClock;

void Schedule::adjustBySyncMsg(Message msg) {/*TODO*/ };

// Scheduling slots tasks
void Schedule::scheduleEndSyncSlotTask(void callback()) {
	DeltaTime deltaTime = longClock.getTicks();
	scheduleTask(callback, deltaTime );
};

void Schedule::scheduleEndWorkSlotTask(void callback()) {};
void Schedule::scheduleEndFishSlotTask(void callback()) {};
void Schedule::scheduleEndMergeSlotTask(void callback()) {};

void Schedule::scheduleStartSyncSlotTask(void callback()) {};
// Work slot follows sync without start callback
void Schedule::scheduleStartFishSlotTask(void callback()) {};
void Schedule::scheduleStartMergeSlotTask(void callback()) {};
