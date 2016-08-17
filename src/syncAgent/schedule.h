
#pragma once


#include "message.h"
#include "longClock.h"

/*
 * Schedule is infinite sequence of periods, aligned with global clock of clique.
 * Period is sequence of slots: sync, work, sleeping, ...., sleeping
 *
 * Sleeping slots may be repurposed for fishing or merging.
 * At most one sleeping slot is repurposed per period
 * (so that use of electrical power is not bursty.)
 *
 * Implementation here is mainly about aligning with global clock.
 *
 * Knowledge (implementation) of slot sequence is spread through SyncAgent.
 * (callbacks know which slot should follow.)
 *
 * The mcu and radio may be sleeping(low-power idle) during any slot, not just sleeping slots.
 * The OS schedules tasks using a low-power timer peripheral that never is off.
 *
 * Because a message may be received after the intended end of a slot
 * (if a msg reception starts during the slot and the radio is not turned off while a msg is in progress.)
 * TODO ???
 *
 * Responsibilities:
 * - maintain period start time (in sync with members of clique)
 * - schedule tasks (interface to OS)
 *
 * Note scheduled tasks run at slot start/end or unaligned
 */
class Schedule {
private:
	static LongClock longClock;
	static LongTime startTimeOfPeriod;

	static const int CountSlots = 20;
	static const DeltaTime SlotDuration = 100;

public:
	void start();

	void adjustBySyncMsg(Message msg);

	// Scheduling slots tasks
	void scheduleEndSyncSlotTask(void callback());
	void scheduleEndWorkSlotTask(void callback());
	void scheduleEndFishSlotTask(void callback());
	void scheduleEndMergeSlotTask(void callback());

	void scheduleStartSyncSlotTask(void callback());
	// Work slot follows sync without start callback
	void scheduleStartFishSlotTask(void callback());
	void scheduleStartMergeSlotTask(void callback());

	// nowTime is not aligned with slot starts.  Result need not be multiple of slotDuration.
	DeltaTime  deltaNowToStartNextSync();
	DeltaTime  deltaStartThisSyncToNow();

	// Times
	LongTime startTimeOfNextPeriod();
	LongTime timeOfThisSyncSlotEnd();	// Of this period
	LongTime timeOfThisWorkSlotEnd();
	LongTime timeOfNextSyncSlotStart();	// Of next period.

};
