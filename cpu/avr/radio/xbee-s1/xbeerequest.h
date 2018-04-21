/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XBEE_REQUEST_H
#define XBEE_REQUEST_H

#include "xbee_const.h"
#include "xbeeaddress.h"
/*---------------------------------------------------------------------------*/
/**
 * Super class of all XBee requests (TX packets)
 * Users should never create an instance of this class; instead use an subclass of this class
 * It is recommended to reuse Subclasses of the class to conserve memory
 * <p/>
 * This class allocates a buffer to
 */
typedef struct XBeeRequest {
	/**
	 * Constructor
	 * TODO make protected
	 */
	void (*XBeeRequest) (uint8_t apiId, uint8_t frameId);
	/**
	 * Sets the frame id.  Must be between 1 and 255 inclusive to get a TX status response.
	 */
	void (*setFrameId) (uint8_t frameId);
	/**
	 * Returns the frame id
	 */
	uint8_t (*getFrameId) ();
	/**
	 * Returns the API id
	 */
	uint8_t (*getApiId) ();
	// setting = 0 makes this a pure virtual function, meaning the subclass must implement, like abstract in java
	/**
	 * Starting after the frame id (pos = 0) and up to but not including the checksum
	 * Note: Unlike Digi's definition of the frame data, this does not start with the API ID.
	 * The reason for this is the API ID and Frame ID are common to all requests, whereas my definition of
	 * frame data is only the API specific data.
	 */
	uint8_t (*getFrameData) (uint8_t pos);
	/**
	 * Returns the size of the api frame (not including frame id or api id or checksum).
	 */
	uint8_t (*getFrameDataLength) (void);
	//void reset();
}XBeeRequest;
	static void setApiId(uint8_t apiId);
	static uint8_t _apiId;
	static uint8_t _frameId;
/*---------------------------------------------------------------------------*/
/**
 * All TX packets that support payloads extend this class
 */
typedef struct PayloadRequest {
    
	void (*PayloadRequest)(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength);
	/**
	 * Returns the payload of the packet, if not null
	 */
	uint8_t* (*getPayload) (void);
	/**
	 * Sets the payload array
	 */
	void (*setPayload) (uint8_t* payloadPtr);
	/**
	 * Returns the length of the payload array, as specified by the user.
	 */
	uint8_t (*getPayloadLength) (void);
	/**
	 * Sets the length of the payload to include in the request.  For example if the payload array
	 * is 50 bytes and you only want the first 10 to be included in the packet, set the length to 10.
	 * Length must be <= to the array length.
	 */
	void (*setPayloadLength) (uint8_t payloadLength);
    struct XBeeRequest;
}PayloadRequest;
	static uint8_t* _payloadPtr;
	static uint8_t _payloadLength;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Series 1 TX packet that corresponds to Api Id: TX_16_REQUEST
 * <p/>
 * Be careful not to send a data array larger than the max packet size of your radio.
 * This class does not perform any validation of packet size and there will be no indication
 * if the packet is too large, other than you will not get a TX Status response.
 * The datasheet says 100 bytes is the maximum, although that could change in future firmware.
 */
typedef struct Tx16Request {
    
	void (*Tx16Request) (uint16_t addr16, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId);
	/**
	 * Creates a Unicast Tx16Request with the ACK option and DEFAULT_FRAME_ID
	 */
	void (*Tx16RequestDefault) (uint16_t addr16, uint8_t *payload, uint8_t payloadLength);
	/**
	 * Creates a default instance of this class.  At a minimum you must specify
	 * a payload, payload length and a destination address before sending this request.
	 */
	uint16_t (*getAddress16) (void);
	void (*setAddress16) (uint16_t addr16);
	uint8_t (*getOption) (void);
	void (*setOption) (uint8_t option);
	uint8_t (*getFrameData) (uint8_t pos);
	uint8_t (*getFrameDataLength) (void);
    struct PayloadRequest;
	
}Tx16Request;
	static uint16_t _addr16;
	static uint8_t _option;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Series 1 TX packet that corresponds to Api Id: TX_64_REQUEST
 *
 * Be careful not to send a data array larger than the max packet size of your radio.
 * This class does not perform any validation of packet size and there will be no indication
 * if the packet is too large, other than you will not get a TX Status response.
 * The datasheet says 100 bytes is the maximum, although that could change in future firmware.
 */
typedef struct Tx64Request {
	void (*Tx64Request) (XBeeAddress64 *addr64, uint8_t option, uint8_t *payload, uint8_t payloadLength, uint8_t frameId); //&
	/**
	 * Creates a unicast Tx64Request with the ACK option and DEFAULT_FRAME_ID
	 */
	void (*Tx64RequestDefault) (XBeeAddress64 *addr64, uint8_t *payload, uint8_t payloadLength); //&
	/**
	 * Creates a default instance of this class.  At a minimum you must specify
	 * a payload, payload length and a destination address before sending this request.
	 */
	XBeeAddress64* (*getAddress64) (void); //&
	void (*setAddress64) (XBeeAddress64* addr64); //&
	// TODO move option to superclass
	uint8_t (*getOption) (void);
	void (*setOption) (uint8_t option);
	uint8_t (*getFrameData) (uint8_t pos);
	uint8_t (*getFrameDataLength) (void);
    struct PayloadRequest;
}Tx64Request;
	static XBeeAddress64 _addr64;
	static uint8_t _option;
/*---------------------------------------------------------------------------*/
#endif //XBee_h