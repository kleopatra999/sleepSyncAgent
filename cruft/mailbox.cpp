
#include "../config.h"
#include "mailbox.h"


#if SYNC_AGENT_IS_LIBRARY==1  && SYNC_AGENT_CONVEY_WORK==1

// platform implements queues

#else

// Is library but no conveyance of work
// Stub out calls to work queues

// stub work queues
// is...Msg returning false means SyncAgent will not call unqueuing and freeing functions

bool isQueuedWorkMsgFromApp() { return false; }
WorkPayload unqueueWorkMsgFromApp() { return 0; }
//void freeWorkMsg(void*) {}


// !!!! Implement received msg queue as get from single, static receive buffer

bool isQueuedReceivedMsg() { return true; }	// Since this is called from the receive IRQ, buffer is filled

#ifdef FUTURE
uint8_t* unqueueReceivedMsg() {
	uint8_t* bufferPtr;
	uint8_t length;

	//radio.getBufferAddressAndLength(&bufferPtr, *length);
	return nullptr;
}
#endif

void freeReceivedMsg(void* msg) { (void) msg; }	// Not a real queue, buffer is static

void queueWorkMsgToApp(void * msg, int length) {(void) msg; (void) length; }


#endif

