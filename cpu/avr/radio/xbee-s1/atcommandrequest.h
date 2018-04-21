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

#ifndef ATCOMMAND_REQUEST_H
#define ATCOMMAND_REQUEST_H

#include "xbee_const.h"
#include "xbeeaddress.h"
/**
 * Represents an AT Command TX packet
 * The command is used to configure the serially connected XBee radio
 */
typedef struct AtCommandRequest {
	void (*AtCommandRequestBegin) (uint8_t *);
	void (*AtCommandRequest) (uint8_t *, uint8_t *, uint8_t);
	uint8_t (*getFrameData) (uint8_t);
	uint8_t (*getFrameDataLength) (void);
	uint8_t* (*getCommand) (void);
	void (*setCommand) (uint8_t*);
	uint8_t* (*getCommandValue) (void);
	void (*setCommandValue) (uint8_t*);
	uint8_t (*getCommandValueLength) (void);
	void (*setCommandValueLength) (uint8_t);
	/**
	 * Clears the optional commandValue and commandValueLength so that a query may be sent
	 */
	void (*clearCommandValue) (void);
	//void reset();
    struct XBeeRequest;
}AtCommandRequest;
	static uint8_t *_command;
	static uint8_t *_commandValue;
	static uint8_t _commandValueLength;

/**
 * Represents an Remote AT Command TX packet
 * The command is used to configure a remote XBee radio
 */
typedef struct RemoteAtCommandRequest {
	/**
	 * Creates a RemoteAtCommandRequest with 16-bit address to set a command.
	 * 64-bit address defaults to broadcast and applyChanges is true.
	 */
	void (*RemoteAtCommandRequestBegin16) (uint16_t remoteAddress16, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength);
	/**
	 * Creates a RemoteAtCommandRequest with 16-bit address to query a command.
	 * 64-bit address defaults to broadcast and applyChanges is true.
	 */
	void (*RemoteAtCommandRequest16) (uint16_t remoteAddress16, uint8_t *command);
	/**
	 * Creates a RemoteAtCommandRequest with 64-bit address to set a command.
	 * 16-bit address defaults to broadcast and applyChanges is true.
	 */
	void (*RemoteAtCommandRequestBegin64) (XBeeAddress64 *remoteAddress64, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength); //&
	/**
	 * Creates a RemoteAtCommandRequest with 16-bit address to query a command.
	 * 16-bit address defaults to broadcast and applyChanges is true.
	 */
	void (*RemoteAtCommandRequest64) (XBeeAddress64 *remoteAddress64, uint8_t *command); //&
	uint16_t (*getRemoteAddress16) (void);
	void (*setRemoteAddress16) (uint16_t remoteAddress16);
	XBeeAddress64* (*getRemoteAddress64) (void); //&
	void (*setRemoteAddress64)(XBeeAddress64 *remoteAddress64); //&
	int (*getApplyChanges)(void);
	void (*setApplyChanges) (int applyChanges);
	uint8_t (*getFrameData) (uint8_t pos);
	uint8_t (*getRemoteFrameDataLength) (void);
    struct AtCommandRequest;
}RemoteAtCommandRequest;
	//	static XBeeAddress64 broadcastAddress64;
	//	static uint16_t broadcast16Address;
	static XBeeAddress64 _remoteAddress64;
	static uint16_t _remoteAddress16;
	static int _applyChanges;
#endif //XBee_h