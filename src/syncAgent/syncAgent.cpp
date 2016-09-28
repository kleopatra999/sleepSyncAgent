
#include <cassert>
#include "syncAgent.h"


// Static data members
bool SyncAgent::isSyncingState = false;
uint8_t SyncAgent::receiveBuffer[255];

Clique SyncAgent::clique;
DropoutMonitor SyncAgent::dropoutMonitor;
CliqueMerger SyncAgent::cliqueMerger;
Role SyncAgent::role;
SyncMessage SyncAgent::outwardSyncMsg;
WorkMessage SyncAgent::workMsg;
Serializer SyncAgent::serializer;
PowerManager SyncAgent::powerMgr;
Sleeper SyncAgent::sleeper;

Radio* SyncAgent::radio;
void (*SyncAgent::onWorkMsgQueuedCallback)();


// This file only implements part of the class, see many other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.

void SyncAgent::init(
		Radio * aRadio,
		void (*aOnWorkMsgQueuedCallback)()
	)
{
	sleeper.init();

	radio = aRadio;
	// Configure radio
	// Connect radio IRQ to sleeper so it knows reason for wake
	radio->init(sleeper.msgReceivedCallback);

	onWorkMsgQueuedCallback = aOnWorkMsgQueuedCallback;

	clique.reset();
	// ensure initial state of SyncAgent
	assert(role.isFisher());
	assert(clique.isSelfMaster());
}


void SyncAgent::xmitSync(SyncMessage& msg) {
	radio->transmitStaticSynchronously();	// blocks until transmit complete
	// TODO serializer.serialize(msg),
		//	Serializer::OnAirSyncMsgPayloadLength);
	// assert transmit complete
}


void SyncAgent::xmitWork(WorkMessage& msg) {
	// TODO (serializer.serialize(msg),
	//			Serializer::OnAirSyncMsgPayloadLength);
	radio->transmitStaticSynchronously();
}


#ifdef OBS
void SyncAgent::startSyncing() {

	assert(! isSyncing);
	// Assert never had sync, or lost sync

	// Alternative: try recovering lost sync
	// Here, brute force: start my own clique

	clique.reset();
	assert(clique.isSelfMaster());
	// clique schedule starts now, at time of this call.
	// clique is not in sync with others, except by chance.

	// OBS scheduleSyncWake();

	// OBS calling app can sleep, wake event onSynchWake()
}


void SyncAgent::resumeAfterPowerRestored() {
	/*
	 * Not reset clique.  Resume previous role and schedule.
	 * If little time has passed since lost power, might still be in sync.
	 * Otherwise self has drifted, and will experience masterDropout.
	 */
	assert(isPaused);
	isPaused = false;
	clique.schedule.resumeAfterPowerRestored();
	scheduleSyncWake();
}
#endif





