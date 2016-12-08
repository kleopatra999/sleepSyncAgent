
#include <cassert>

#include "../globals.h" // fishPolicy

#include "schedule.h"
#include "../scheduleParameters.h"	// probably already included by MergeOffset


// static singleton data

LongClock Schedule::longClock;	// has-a

// Set when SyncPeriod starts, so invariant: in the past
LongTime Schedule::startTimeOfSyncPeriod;

/*
 * !!! Can change during current period.  See adjustBySyncMsg()
 * Can be in future, advances forward in time.
 */
LongTime Schedule::endTimeOfSyncPeriod;

// Remembered during fish slot
LongTime Schedule::memoStartTimeOfFishSlot;



void Schedule::startFreshAfterHWReset(){
	log("Schedule reset\n");
	longClock.reset();
	startTimeOfSyncPeriod = longClock.nowTime();	// Must do this to avoid assertion in rollPeriodForwardToNow
	rollPeriodForwardToNow();
	// Out of sync with other cliques
}


/*
 * Called at wall time that should be SyncPoint.
 * That is, called when a timer expires that indicates end of sync period and time to begin next.
 *
 * There are cases when this is called late:
 * 1) Since we can fish in the last slot before this time, and fishing code may delay us a short time,
 * this may be called a short time later than usual.
 * If not a short time, is error in algorithm.
 *
 * 2) There could be other faults that delay a call to this (timer does not expire at correct wall time.)
 * E.G. there is a mysterious delay when an invalid CRC is detected.
 *
 * 3) Also, using the debugger could delay timer expiration.
 *
 * In those cases, if the delay is not short, sync is lost
 * (the wall time of our SyncPoint not the same as other clique member's wall time of SyncPoint.)
 * But it is unavoidable for a robust algorithm.  See next paragraph.
 *
 * FLAWED design if called late:
 *    startTimeOfSyncPeriod = endTimeOfSyncPeriod;
 *    endTimeOfSyncPeriod = startTimeOfSyncPeriod + NormalSyncPeriodDuration;
 * That yields flawed scheduling (scheduling events in the past or at zero timeout) later
 * since it leaves the startTimeOfSyncPeriod much in the past.
 * It yields setting timers for times in the past, which should expire immediately.
 */
void Schedule::rollPeriodForwardToNow() {

	LongTime startOfPreviousSyncPeriod = startTimeOfSyncPeriod;

	// Remember startTime as nowTime.  See above.  If called late, sync might be lost.
	startTimeOfSyncPeriod = longClock.nowTime();
	endTimeOfSyncPeriod = startTimeOfSyncPeriod + ScheduleParameters::NormalSyncPeriodDuration;

	/*
	 * assert startTimeOfSyncPeriod is close to nowTime().
	 * This is called at the time that should be SyncPoint.
	 * But since we can fish in the last slot before this time,
	 * and fishing may delay us a short time,
	 * this may be called a short time later than usual.
	 * If not a short time, is error in algorithm.
	 */
	/*
	 * !!! This assertion can't be stepped-in while debugging
	 * since the RTC continues to run while you are stepping.
	 */
	//assert( longClock.timeDifferenceFromNow(startTimeOfSyncPeriod) < ScheduleParameters::SlotDuration );
}


/*
 * Crux
 *
 * Called from a sync, work, or fish slot:
 * - most often, usually, a MasterSync in a Sync slot
 * - rarely, a MergeSync in Sync slot
 * - very rarely, a MasterSync in a Fish slot
 *
 * A sync message adds to ***end*** of period (farther into the future).
 * TODO not true for a MasterSync in SyncSlot?
 *
 * Otherwise, for a fish slot, the calculation of the end of the fish slot
 * (based on the start) might be beyond the end of sync period?
 * (But a fish slot ends on the first msg, so not a concern?)
 *
 * It is not trivial to schedule end of period in the future,
 * since we might be near the current end of the period already.
 * assert startTimeOfSyncPeriod < nowTime()  < endTimeOfSyncPeriod
 * i.e. now in the current period, but may be near the end of it,
 * or beyond it because of delay in processing fished SyncMessage?
 *
 * assert not much time has elapsed since msg arrived.
 * I.E. nowTime() is approximate TOA of SyncMessage.
 * For more accuracy, we could timestamp msg arrival as early as possible.
 */
void Schedule::adjustBySyncMsg(SyncMessage* msg) {
	/*
	 * assert endSyncSlot or endFishSlot has not yet occurred, but this doesn't affect that.
	 */

	logInt(msg->deltaToNextSyncPoint.get()); log(":Adj sched by\n");
	logInt(deltaNowToNextSyncPoint()); log(":Delta to next sync\n");

	// FUTURE optimization?? If adjustedEndTime is near old endTime, forego setting it?
	endTimeOfSyncPeriod = adjustedEndTime(msg->deltaToNextSyncPoint);

	// assert old startTimeOfSyncPeriod < new endTimeOfSyncPeriod  < nowTime() + 2*periodDuration

	// Not schedule end time too far from remembered start time.
	assert( (endTimeOfSyncPeriod - startTimeOfSyncPeriod) < 2* ScheduleParameters::NormalSyncPeriodDuration);
}

/*
 * ALTERNATIVE DESIGN: NOT IMPLEMENTED
 * Adjusted end time of SyncPeriod, where SyncPeriod is never shortened by much, only lengthened.
 * "By much" means: not more than one slot before current end time of SyncPeriod.
 * Here, elsewhere there must be code to insure that fishing in the last slot
 * and MergeSync transmit does not fall outside the shortened SyncPeriod.


	DeltaTime adjustment;
	if (senderDeltaToSyncPoint > SlotDuration) {
		// Self is not already near end of adjusted SyncPeriod
		adjustment = senderDeltaToSyncPoint;
 */
/*
 * Adjusted end time of SyncPeriod, where SyncPeriod is never shortened (at all), only lengthened.
 */
LongTime Schedule::adjustedEndTime(DeltaSync deltaSync) {
	LongTime result = longClock.nowTime() + deltaSync.get();
	if (result < timeOfNextSyncPoint()) {
		result += ScheduleParameters::NormalSyncPeriodDuration;
	}
	assert( result < 2 * ScheduleParameters::NormalSyncPeriodDuration);
	assert( result > timeOfNextSyncPoint());
	return result;
}


/*
 * Deltas
 *
 * uint32 math is modulo: a delta is correct if laterTime > earlierTime
 * (even if the uint32 clock has wrapped).
 * But is incorrect if earlierTime > laterTime.
 * So if you are calculating a timeout to a future deadline time,
 * and the deadline is already in the past, simple math on uint32 is wrong.
 * Use a LongClock to avoid the issue.
 *
 * All these involve a conversion to OSTime i.e. take lower 32 bits of result.
 *
 * If zero, a sleep...(0) executes immediately without delay.
 * Thus these can safely return 0.
 */
//DeltaTime  Schedule::deltaNowToStartNextSync() {


DeltaTime  Schedule::deltaNowToNextSyncPoint() {
	return longClock.clampedTimeDifferenceFromNow(timeOfNextSyncPoint());
}

// Different: backwards from others: from past time to now
DeltaTime  Schedule::deltaPastSyncPointToNow() {
	DeltaTime result = longClock.clampedTimeDifferenceToNow(startTimeOfSyncPeriod);
	/*
	 * This can only be called when now is within current sync period,
	 * hence result must be less than sync period duration
	 */
	assert(result <= ScheduleParameters::NormalSyncPeriodDuration);
	return result;
}

DeltaTime Schedule::deltaToThisSyncSlotMiddleSubslot(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisSyncSlotMiddleSubslot());
}
DeltaTime Schedule::deltaToThisSyncSlotEnd(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisSyncSlotEnd());
}

// We don't need WorkSlot start
DeltaTime Schedule::deltaToThisWorkSlotMiddle(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisWorkSlotMiddle());
}
DeltaTime Schedule::deltaToThisWorkSlotEnd(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd());
}

DeltaTime Schedule::deltaToThisFishSlotStart(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisFishSlotStart());
}
DeltaTime Schedule::deltaToThisFishSlotEnd(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisFishSlotEnd());
}

DeltaTime Schedule::deltaToThisMergeStart(MergeOffset offset){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisMergeStart(offset.get()));
}


DeltaTime Schedule::rampupDelay() {
#ifdef NRF52
	// ramp up in fast mode is 40uSec, i.e. 1.3 ticks
	return 2;
#else // NRF51
	// ramp up is 130 uSec i.e. 4.3 ticks
	return 4;
#endif
}
/*
 * The offset a Work message would hold (if offset were not used otherwise)
 * Used to mangle a Work message to be equivalent to a Sync.
 * A Work message is sent 1-1/2 slots from SyncPoint.
 */
DeltaTime Schedule::deltaFromWorkMiddleToEndSyncPeriod(){
	return ScheduleParameters::NormalSyncPeriodDuration - ScheduleParameters::SlotDuration - halfSlotDuration();
}


/*
 * Times
 *
 * These return future times, as long as called at appropriate instant
 * and task processing times is smaller than slotDurations.
 * When these return past times, calculation of DeltaTime clamps to zero.
 *
 * These may be called many times during a slot (to schedule a new Timer),
 * not just at the start of a slot.
 * Thus you cannot assert they are greater than nowTime().
 */


// Next
LongTime Schedule::timeOfNextSyncPoint() {
	return endTimeOfSyncPeriod;
}

/*
 * Start of period and start of SyncSlot coincide.
 * FUTURE: Choice of sync slot at start of period is arbitrary, allow it to be anywhere in period?
 */

LongTime Schedule::timeOfThisSyncSlotMiddleSubslot() {
	return startTimeOfSyncPeriod + halfSlotDuration() - rampupDelay() ;
}


LongTime Schedule::timeOfThisSyncSlotEnd() { return startTimeOfSyncPeriod + ScheduleParameters::SlotDuration; }

// WorkSlot immediately after SyncSlot
LongTime Schedule::timeOfThisWorkSlotMiddle() { return startTimeOfSyncPeriod + ScheduleParameters::SlotDuration + halfSlotDuration(); }
LongTime Schedule::timeOfThisWorkSlotEnd() { return startTimeOfSyncPeriod + 2 * ScheduleParameters::SlotDuration; }


/*
 * Fish slot:
 * - starts at slot normally sleeping.
 * - Ends after remembered start.
 *
 * Start time is calculated at instant:  just after end of work slot.
 * Time til start is in [0, timeTilLastSleepingSlot]
 */
LongTime Schedule::timeOfThisFishSlotStart() {
	// policy chooses which normally sleeping slots to fish in.
	ScheduleCount sleepingSlotOrdinal = fishPolicy.next();
	// minus 1: convert ordinal to zero-based duration multiplier
	LongTime result = startTimeOfSyncPeriod +  (sleepingSlotOrdinal - 1) * ScheduleParameters::SlotDuration;

	/*
	 * Since some cpu cycles have elapsed after end of previous slot,
	 * startTimeOfFishSlot can be < longClock.nowTime().
	 * In other words, the case where chosen slot (to fish in) is slot immediately following previous slot,
	 * and time to start fish slot is already past.
	 * In this case, the subsequently calculated timeout will be zero,
	 * and there will be no sleep.
	 */

#ifdef FUTURE
	??? Dubious code.
	/*
	 * Time to start fish slot must be no later than start time of last sleeping slot,
	 * else we won't start next sync period on time.
	 */
	LongTime nextSyncPoint = timeOfNextSyncPoint();
	assert(result <= (nextSyncPoint - ScheduleParameters::SlotDuration + 10*ScheduleParameters::MsgDurationInTicks));
#endif

	/*
	 * SyncPeriod is never shortened by adjustment.
	 * Hence result must be less than timeOfNextSyncPoint,
	 * else not enough time to perform a FishSlot without delaying end of SyncPeriod.
	 */
	assert(result > timeOfNextSyncPoint() );

	// !!! Memoize
	memoStartTimeOfFishSlot = result;

	return result;
}

LongTime Schedule::timeOfThisFishSlotEnd() {
	LongTime result = memoStartTimeOfFishSlot + ScheduleParameters::SlotDuration;

	// A Fish slot can be the last slot
	// Fish slot should not end after next SyncPoint
	LongTime nextSyncPoint = timeOfNextSyncPoint();
	if (result > nextSyncPoint) {
		log("End fish slot at sync point\n");
		result = nextSyncPoint;
	}

	// result may be < nowTime() i.e. in the past
	// in which case delta==0 and sleepUntilTimeout(delta) will timeout immediately.
	return result;
}



/*
 * Merge slot:
 * Only start of slot is needed, not the end (slot ends when MergeSynce is xmitted.)
 *
 * offset comes from cliqueMerger.mergeOffset
 */
LongTime Schedule::timeOfThisMergeStart(DeltaTime offset) {
	LongTime result;
	result = startTimeOfSyncPeriod + offset;
	assert(result < endTimeOfSyncPeriod);
	return result;
}


#ifdef OBSOLETE

//LongTime Schedule::timeOfNextSyncSlotStart() { return timeOfNextSyncPeriodStart(); }


/*
 * Duration of this SyncPeriod, possibly as adjusted.
 */
DeltaTime Schedule::thisSyncPeriodDuration() {

}
#endif
