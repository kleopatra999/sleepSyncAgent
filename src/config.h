/*
 * Configure:
 * - optional behaviour of SyncAgent
 * - library build
 */

//TODO rename idle near brownout
/*
 * Define this if SyncAgent should conserve power
 *
 * Yes:
 * SyncAgent checks Vcc and will not maintain sync or convey work if Vcc is low.
 * SyncAgent will attempt to recover last known sync when power is sufficient.
 *
 * Platform must implement getVcc()
 *
 * No:
 * SyncAgent maintains sync even to the point of brownout.
 * Upon brownout exception (which app handles) typically mcu resets,
 * all volatile memory is lost, including sync.
 *
 * isPowerForRadio() is stubbed to always return true.
 *
 * Platform layer need not implement getVcc()
 */
//#define SYNC_AGENT_CONSERVE_POWER 1




/*
 * Define equal to 1 if this if SyncAgent should convey work messages to app.
 *
 * Yes:
 * SyncAgent unqueues and enqueues to work queues to/from app.
 *
 * No:
 * SyncAgent does not implement a work slot.
 * SyncAgent work enqueues and dequeues are stubbed to no ops.
 * Platform layer need not implement work queue operations.
 * The app doesn't communicate work with other units.
 *
 * All the app does is receive a callback at each SyncPoint.
 * The callback should be a lower priority process than the SyncAgent (or kept short.)
 */
#define SYNC_AGENT_CONVEYS_WORK 1


/*
 * Define this if SyncAgent is built as a library.
 *
 * Yes:
 * SyncAgent expects platform to implement certain functions (undefined in the library.)
 *
 * No:
 * Stubs are compiled in so project builds and links cleanly
 * (but it is non-functional.)
 */
//#define SYNC_AGENT_IS_LIBRARY 1



/*
 * Define this if, on the platform, the radio peripheral is synchronous to the SyncAgent.
 *
 * Note the radio peripheral is usually an independent peripheral/device,
 * but the platform may layer it so that messages are synchronous.
 * A sleeping SyncAgent only means the mcu is idle, the radio peripheral may still be active.
 *
 * Yes: (synchronous)
 * After platform receives msg, radio is disabled until SyncAgent starts it again.
 * When platform receives msg:
 * - sleeping sync agent wakes because it was sleeping on ANY event
 * - SyncAgent can know the reason for wakening by calling isWakeReasonMsgReceived()
 * - SyncAgent must read the received message before restarting the receiver.
 *
 *
 * No:
 * Radio peripheral is run in a separate thread of an RTOS
 * After platform receives msg, radio continues to receive without intervention by SyncAgent,
 * whether or not SyncAgent keeps up.
 * When platform receives msg:
 *  - it is queued
 *  - sleeping sync agent wakes because it was blocked on the queue
 */
#define RECEIVE_IS_SYNCHRONOUS 1



