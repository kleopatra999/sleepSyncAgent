
#pragma once


/*
 * Crux types are DeltaSync and MergeOffset.
 * LongTime and DeltaTime are important but conventional.
 */
#include "message.h"	// SyncMessage, DeltaSync
#include "../../augment/timeMath.h"	// LongTime, DeltaTime
#include "mergeOffset.h"


/*
 * Schedule is infinite sequence of periods, aligned with global clock of clique.
 * Period is sequence of slots: sync, work, sleeping, ...., sleeping
 *
 * Sleeping slots may be repurposed for fishing or merging.
 * At most one sleeping slot is repurposed per period
 * (so that use of electrical power is not bursty.)
 *
 * Division into slots is an ideal.
 * Many sleeping slots go by without any marking by the algorithm.
 * Merge-repurposed slots need not be aligned with my slot divisions
 * (since they are talking to an other units slot divisions.)
 *
 * Implementation here is mainly about aligning with global clock.
 *
 * Knowledge (implementation) of slot sequence is found in syncAgentLoop.cpp, doSyncPeriod().
 *
 * Power:
 * The mcu may be sleeping(low-power idle) during any slot, not just sleeping slots.
 * The radio is off during unrepurposed sleeping slots.
 * The radio is on (xmitting or rcving) during Sync and Work slots.
 * The radio is on (rcving) during Fish repurposed slot.
 * The radio is briefly on (xmitting) during a Merge repurposed slot.
 * The OS schedules tasks using a low-power timer peripheral that never is off.
 *
 * FUTURE message may be received after the intended end of a slot??? or race, msg received after radio off ?
 * (if a msg reception starts during the slot and the radio is not turned off while a msg is in progress.)
 *
 *
 * Responsibilities:
 * - own an infinite duration clock based on OSClock()
 * - maintain period start time (in sync with members of clique)
 * - schedule tasks (interface to platform or OS)
 *
 * Note scheduled tasks run at slot start/end or unaligned
 *
 * !!! Be careful with time math.
 * Compiler will convert longer ints to shorter quietly, with possible loss of data.
 * OS scheduling requires a DeltaTime (known as a timeout in some RTOS) from now.
 * OS scheduling does NOT take a LongTime, but compiler does not give warning
 * (as this code is currently written.)
 *
 * Power:
 * When power is low, SyncAgent returns to app without scheduling next period.
 * When power is restored, SyncAgent's schedule may still be in sync and schedule is resumed.
 * The LongClock continues to be accurate (since it is derived from a hw clock that continues.)
 * Resuming schedule means taking the last known startTimeOfPeriod
 * (which is more than one period in the past)
 * and advancing it to now by a multiple of PeriodDuration.
 *
 * Terminology:
 * SyncPoint: point in time between consecutive SyncPeriods.
 *
 * !!!
 * SyncAgent owns Clique owns Schedule owns LongClock which uses OSClock() platform function
 * which depends on the Timer owned by Sleeper owned by SyncAgent.
 * In other words, there is a deep unstated connection between LongClock and Timer.
 */
class Schedule {

public:
	/*
	 * aka init().
	 * Set start of sync period to now, essentially a random time in relation to all other units.
	 * This should only be called once:  after that, whatever schedule we are on
	 * is the best estimate of other unit's schedule.
	 */
	static void startFreshAfterHWReset();

	// FUTURE static void resumeAfterPowerRestored();

	static void rollPeriodForwardToNow();
	static void adjustBySyncMsg(SyncMessage* msg);
	static LongTime adjustedEndTime(DeltaSync senderDeltaToSyncPoint);	// <<<<
	static LongTime startTimeOfSyncPeriod();

	/*
	 * Deltas from past time to now.
	 *
	 * nowTime is not aligned with slot starts.  Result need not be multiple of slotDuration.
	 */
	static DeltaTime deltaPastSyncPointToNow();

	/*
	 * Deltas from now to future time.
	 * Positive or zero and < SyncPeriodDuration
	 */
	static DeltaTime deltaNowToNextSyncPoint();

	// deltas to slots


	static DeltaTime deltaToThisWorkSlotMiddle();
	static DeltaTime deltaToThisWorkSlotEnd();

	static DeltaTime deltaToThisMergeStart(const MergeOffset* const offset);	 // <<<<

	static DeltaTime deltaFromWorkMiddleToEndSyncPeriod();


	static LongTime nowTime();
	static LongTime timeOfNextSyncPoint();

	// slot times




	static LongTime timeOfThisWorkSlotMiddle();
	static LongTime timeOfThisWorkSlotEnd();


	static LongTime timeOfThisMergeStart(DeltaTime offset);

	static void recordMsgArrivalTime();
	static LongTime getMsgArrivalTime();

};
