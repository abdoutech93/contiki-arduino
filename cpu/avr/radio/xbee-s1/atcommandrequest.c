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

#include "atcommandrequest.h"
#include "xbee_const.h"
#include "xbeeaddress.h"
/*---------------------------------------------------------------------------*/
void AtCommandRequest(uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) {
	XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID);
	_command = command;
	_commandValue = commandValue;
	_commandValueLength = commandValueLength;
}
/*---------------------------------------------------------------------------*/
void AtCommandRequestBegin(uint8_t *command){
	XBeeRequest(AT_COMMAND_REQUEST, DEFAULT_FRAME_ID);
	_command = command;
	clearCommandValue();
}
/*---------------------------------------------------------------------------*/
uint8_t* getCommand() {
	return _command;
}
/*---------------------------------------------------------------------------*/
uint8_t* getCommandValue() {
	return _commandValue;
}
/*---------------------------------------------------------------------------*/
uint8_t getCommandValueLength() {
	return _commandValueLength;
}
/*---------------------------------------------------------------------------*/
void setCommand(uint8_t* command) {
	_command = command;
}
/*---------------------------------------------------------------------------*/
void setCommandValue(uint8_t* value) {
	_commandValue = value;
}
/*---------------------------------------------------------------------------*/
void setCommandValueLength(uint8_t length) {
	_commandValueLength = length;
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameData(uint8_t pos) {

	if (pos == 0) {
		return _command[0];
	} else if (pos == 1) {
		return _command[1];
	} else {
		return _commandValue[pos - AT_COMMAND_API_LENGTH];
	}
}
/*---------------------------------------------------------------------------*/
void clearCommandValue() {
	_commandValue = NULL;
	_commandValueLength = 0;
}
/*---------------------------------------------------------------------------*/
RemoteAtCommandRequest.broadcastAddress64 = XBeeAddress64(0x0, BROADCAST_ADDRESS);
void RemoteAtCommandRequest() {
    AtCommandRequest(NULL, NULL, 0);
	_remoteAddress16 = 0;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}
/*---------------------------------------------------------------------------*/
RemoteAtCommandRequestBegin16(uint16_t remoteAddress16, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) {
    AtCommandRequest(command, commandValue, commandValueLength);
	_remoteAddress64 = broadcastAddress64;
	_remoteAddress16 = remoteAddress16;
	_applyChanges = true;
	setApiId(REMOTE_AT_REQUEST);
}
/*---------------------------------------------------------------------------*/
RemoteAtCommandRequest16(uint16_t remoteAddress16, uint8_t *command) {
    AtCommandRequest(command, NULL, 0);
	_remoteAddress64 = broadcastAddress64;
	_remoteAddress16 = remoteAddress16;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}
/*---------------------------------------------------------------------------*/
RemoteAtCommandRequestBegin64(XBeeAddress64 &remoteAddress64, uint8_t *command, uint8_t *commandValue, uint8_t commandValueLength) {
    AtCommandRequest(command, commandValue, commandValueLength);
	_remoteAddress64 = remoteAddress64;
	// don't worry.. works for series 1 too!
	_remoteAddress16 = ZB_BROADCAST_ADDRESS;
	_applyChanges = true;
	setApiId(REMOTE_AT_REQUEST);
}
/*---------------------------------------------------------------------------*/
RemoteAtCommandRequest64(XBeeAddress64 &remoteAddress64, uint8_t *command) {
    AtCommandRequest(command, NULL, 0);
	_remoteAddress64 = remoteAddress64;
	_remoteAddress16 = ZB_BROADCAST_ADDRESS;
	_applyChanges = false;
	setApiId(REMOTE_AT_REQUEST);
}
/*---------------------------------------------------------------------------*/
uint16_t getRemoteAddress16() {
	return _remoteAddress16;
}
/*---------------------------------------------------------------------------*/
void setRemoteAddress16(uint16_t remoteAddress16) {
	_remoteAddress16 = remoteAddress16;
}
/*---------------------------------------------------------------------------*/
XBeeAddress64& getRemoteAddress64() {
	return _remoteAddress64;
}
/*---------------------------------------------------------------------------*/
void setRemoteAddress64(XBeeAddress64 &remoteAddress64) {
	_remoteAddress64 = remoteAddress64;
}
/*---------------------------------------------------------------------------*/
bool getApplyChanges() {
	return _applyChanges;
}
/*---------------------------------------------------------------------------*/
void setApplyChanges(bool applyChanges) {
	_applyChanges = applyChanges;
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameData(uint8_t pos) {
	if (pos == 0) {
		return (_remoteAddress64.getMsb() >> 24) & 0xff;
	} else if (pos == 1) {
		return (_remoteAddress64.getMsb() >> 16) & 0xff;
	} else if (pos == 2) {
		return (_remoteAddress64.getMsb() >> 8) & 0xff;
	} else if (pos == 3) {
		return _remoteAddress64.getMsb() & 0xff;
	} else if (pos == 4) {
		return (_remoteAddress64.getLsb() >> 24) & 0xff;
	} else if (pos == 5) {
		return (_remoteAddress64.getLsb() >> 16) & 0xff;
	} else if (pos == 6) {
		return(_remoteAddress64.getLsb() >> 8) & 0xff;
	} else if (pos == 7) {
		return _remoteAddress64.getLsb() & 0xff;
	} else if (pos == 8) {
		return (_remoteAddress16 >> 8) & 0xff;
	} else if (pos == 9) {
		return _remoteAddress16 & 0xff;
	} else if (pos == 10) {
		return _applyChanges ? 2: 0;
	} else if (pos == 11) {
		return getCommand()[0];
	} else if (pos == 12) {
		return getCommand()[1];
	} else {
		return getCommandValue()[pos - REMOTE_AT_COMMAND_API_LENGTH];
	}
}
/*---------------------------------------------------------------------------*/
//void AtCommandRequest::reset() {
//	 XBeeRequest::reset();
//}
/*---------------------------------------------------------------------------*/
uint8_t getFrameDataLength() {
	// command is 2 byte + length of value
	return AT_COMMAND_API_LENGTH + _commandValueLength;
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
uint8_t getRemoteFrameDataLength() {
	return REMOTE_AT_COMMAND_API_LENGTH + getCommandValueLength();
}
/*---------------------------------------------------------------------------*/