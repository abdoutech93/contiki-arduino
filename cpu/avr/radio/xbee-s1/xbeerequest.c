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
#include "xbee_const.h"
#include "xbeerequest.h"
/*---------------------------------------------------------------------------*/
// it's peanut butter jelly time!!

XBeeRequest(uint8_t apiId, uint8_t frameId) {
	_apiId = apiId;
	_frameId = frameId;
}
/*---------------------------------------------------------------------------*/
void setFrameId(uint8_t frameId) {
	_frameId = frameId;
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameId() {
	return _frameId;
}
/*---------------------------------------------------------------------------*/
uint8_t getApiId() {
	return _apiId;
}
/*---------------------------------------------------------------------------*/
void setApiId(uint8_t apiId) {
	_apiId = apiId;
}
/*---------------------------------------------------------------------------*/
PayloadRequest(uint8_t apiId, uint8_t frameId, uint8_t *payload, uint8_t payloadLength) : XBeeRequest(apiId, frameId) {
	_payloadPtr = payload;
	_payloadLength = payloadLength;
}
/*---------------------------------------------------------------------------*/
uint8_t* getPayload() {
	return _payloadPtr;
}
/*---------------------------------------------------------------------------*/
void setPayload(uint8_t* payload) {
	_payloadPtr = payload;
}
/*---------------------------------------------------------------------------*/
uint8_t getPayloadLength() {
	return _payloadLength;
}
/*---------------------------------------------------------------------------*/
void setPayloadLength(uint8_t payloadLength) {
	_payloadLength = payloadLength;
}
/*---------------------------------------------------------------------------*/
Tx16Request(uint16_t addr16, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId) : PayloadRequest(TX_16_REQUEST, frameId, data, dataLength) {
	_addr16 = addr16;
	_option = option;
}
/*---------------------------------------------------------------------------*/
Tx16Request(uint16_t addr16, uint8_t *data, uint8_t dataLength) : PayloadRequest(TX_16_REQUEST, DEFAULT_FRAME_ID, data, dataLength) {
	_addr16 = addr16;
	_option = ACK_OPTION;
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameData(uint8_t pos) {

	if (pos == 0) {
		return (_addr16 >> 8) & 0xff;
	} else if (pos == 1) {
		return _addr16 & 0xff;
	} else if (pos == 2) {
		return _option;
	} else {
		return getPayload()[pos - TX_16_API_LENGTH];
	}
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameDataLength() {
	return TX_16_API_LENGTH + getPayloadLength();
}
/*---------------------------------------------------------------------------*/
uint16_t getAddress16() {
	return _addr16;
}
/*---------------------------------------------------------------------------*/
void setAddress16(uint16_t addr16) {
	_addr16 = addr16;
}
/*---------------------------------------------------------------------------*/
uint8_t getOption() {
	return _option;
}
/*---------------------------------------------------------------------------*/
void setOption(uint8_t option) {
	_option = option;
}
/*---------------------------------------------------------------------------*/
void Tx64Request(XBeeAddress64 &addr64, uint8_t option, uint8_t *data, uint8_t dataLength, uint8_t frameId) {
    PayloadRequest(TX_64_REQUEST, frameId, data, dataLength);
	_addr64 = addr64;
	_option = option;
}
/*---------------------------------------------------------------------------*/
void Tx64RequestDefault(XBeeAddress64 &addr64, uint8_t *data, uint8_t dataLength){
    PayloadRequest(TX_64_REQUEST, DEFAULT_FRAME_ID, data, dataLength) ;
	_addr64 = addr64;
	_option = ACK_OPTION;
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameData(uint8_t pos) {

	if (pos == 0) {
		return (_addr64.getMsb() >> 24) & 0xff;
	} else if (pos == 1) {
		return (_addr64.getMsb() >> 16) & 0xff;
	} else if (pos == 2) {
		return (_addr64.getMsb() >> 8) & 0xff;
	} else if (pos == 3) {
		return _addr64.getMsb() & 0xff;
	} else if (pos == 4) {
		return (_addr64.getLsb() >> 24) & 0xff;
	} else if (pos == 5) {
		return (_addr64.getLsb() >> 16) & 0xff;
	} else if (pos == 6) {
		return(_addr64.getLsb() >> 8) & 0xff;
	} else if (pos == 7) {
		return _addr64.getLsb() & 0xff;
	} else if (pos == 8) {
		return _option;
	} else {
		return getPayload()[pos - TX_64_API_LENGTH];
	}
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameDataLength() {
	return TX_64_API_LENGTH + getPayloadLength();
}
/*---------------------------------------------------------------------------*/
XBeeAddress64& getAddress64() {
	return _addr64;
}
/*---------------------------------------------------------------------------*/
void setAddress64(XBeeAddress64& addr64) {
	_addr64 = addr64;
}
/*---------------------------------------------------------------------------*/
uint8_t getOption() {
	return _option;
}
/*---------------------------------------------------------------------------*/
void setOption(uint8_t option) {
	_option = option;
}
/*---------------------------------------------------------------------------*/