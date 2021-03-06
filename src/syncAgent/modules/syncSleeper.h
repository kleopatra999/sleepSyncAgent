
#pragma once

/*
 * Understands sleeping for sync algorithm.
 *
 * Responsibilities:
 * - sleep until a timeout
 * - sleep until either a timeout or valid message received and accepted
 * - filter invalid message packets received
 *
 * A main concern is that the platform Sleeper may wake unexpectedly on events.
 * This code guarantees that timeout has elapsed, even in face of unexpected wake.
 */


typedef void (*voidFuncPtr)();

typedef bool (*DispatchFuncPtr)(SyncMessage *);


class SyncSleeper {

public:
	static void init(
			OSTime maxSaneTimeout,
			LongClockTimer *);

	static void clearReasonForWake();

	static void sleepUntilTimeout(OSTime (*func)());

	static bool sleepUntilMsgAcceptedOrTimeout(
			DispatchFuncPtr,	// Slot*,
			OSTime (*func)());

	static voidFuncPtr getMsgReceivedCallback();
};
