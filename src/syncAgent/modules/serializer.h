#pragma once

#include <inttypes.h>

#include <nRF5x.h>	// BufferPointer
#include "message.h"


/*
 * Serialize: convert data stream to object.
 *
 * Knows order and size of data elements in stream (message payload.)
 * type: 1
 * masterID: 6
 * syncOffset: 3   (OSTime is 24-bit. 2 is max of 128k ticks)
 *
 * !!! This assumes:
 * - the radio is half-duplex (can't xmit and receive at the same time)
 * - SyncAgent is single message at a time (wireless stack does not chain or auto-send e.g. acks)
 * Thus:
 * - knows one radioBuffer, i.e. pointer to data is not passed to methods.
 * - owns common Messages
 *
 * FUTURE: use queues and multiple live messages
 */

class Serializer {

public:
	// Owns Message instances, and returns address of them
	static SyncMessage inwardCommonSyncMsg;
	static SyncMessage outwardCommonSyncMsg;

	// Also knows Radio's buffer, see anon namespace

	// Methods
	static void init(BufferPointer radioBuffer, uint8_t aBufferSize);

	/*
	 * unserialize from radio buffer into Message instance.
	 * returns pointer to polymorphous instance, or nullptr.
	 * nullptr when Message.type is not valid.
	 */
	static SyncMessage* unserialize();

	/*
	 * Serialize from Message instance into radio buffer.
	 */
	static void serializeOutwardCommonSyncMessage();

	// Does contents of Serializer's buffer minimally seem like a Message?
	static bool bufferIsSane();
};
