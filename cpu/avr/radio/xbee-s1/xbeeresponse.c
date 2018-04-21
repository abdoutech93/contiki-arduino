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
#include "xbeeresponse.h"
/*---------------------------------------------------------------------------*/
uint8_t getApiId() {
	return _apiId;
}
/*---------------------------------------------------------------------------*/
void setApiId(uint8_t apiId) {
	_apiId = apiId;
}
/*---------------------------------------------------------------------------*/
uint8_t getMsbLength() {
	return _msbLength;
}
/*---------------------------------------------------------------------------*/
void setMsbLength(uint8_t msbLength) {
	_msbLength = msbLength;
}
/*---------------------------------------------------------------------------*/
uint8_t getLsbLength() {
	return _lsbLength;
}
/*---------------------------------------------------------------------------*/
void setLsbLength(uint8_t lsbLength) {
	_lsbLength = lsbLength;
}
/*---------------------------------------------------------------------------*/
uint8_t getChecksum() {
	return _checksum;
}
/*---------------------------------------------------------------------------*/
void setChecksum(uint8_t checksum) {
	_checksum = checksum;
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameDataLength() {
	return _frameLength;
}
/*---------------------------------------------------------------------------*/
void setFrameLength(uint8_t frameLength) {
	_frameLength = frameLength;
}
/*---------------------------------------------------------------------------*/
bool isAvailable() {
	return _complete;
}
/*---------------------------------------------------------------------------*/
void setAvailable(bool complete) {
	_complete = complete;
}
/*---------------------------------------------------------------------------*/
bool isError() {
	return _errorCode > 0;
}
/*---------------------------------------------------------------------------*/
uint8_t getErrorCode() {
	return _errorCode;
}
/*---------------------------------------------------------------------------*/
void setErrorCode(uint8_t errorCode) {
	_errorCode = errorCode;
}
/*---------------------------------------------------------------------------*/
uint8_t getDataFrom(int index) {
	return getFrameData()[getDataOffset() + index];
}
/*---------------------------------------------------------------------------*/
uint8_t* getData() {
	return getFrameData() + getDataOffset();
}
/*---------------------------------------------------------------------------*/
uint8_t getFrameId() {
	return getFrameData()[0];
}
/*---------------------------------------------------------------------------*/
uint8_t getStatus() {
	return getFrameData()[0];
}
/*---------------------------------------------------------------------------*/
uint8_t getDataLength() {
	return getPacketLength() - getDataOffset() - 1;
}
/*---------------------------------------------------------------------------*/
uint8_t getDataOffset() {
	return getRssiOffset() + 2;
}
/*---------------------------------------------------------------------------*/
void Rx64Response(){
    RxResponse();
	_remoteAddress = XBeeAddress64();
}
/*---------------------------------------------------------------------------*/
/*uint16_t getRemoteAddress16() {
	uint8_t* buffer = getFrameData();
    return (buffer[0] << 8) + buffer[1];
}*/
/*---------------------------------------------------------------------------*/
uint16_t getRemoteAddress16() {
    uint8_t* buffer = getFrameData();
	return (uint16_t)((buffer[0] << 8) + buffer[1]);
}
/*---------------------------------------------------------------------------*/
uint8_t getRssiOffset() {
	return 2;
}
/*---------------------------------------------------------------------------*/
uint8_t getRssiOffset() {
	return 8;
}
/*---------------------------------------------------------------------------*/
uint8_t getRssiOffset() {
	return RX_16_RSSI_OFFSET;
}
/*---------------------------------------------------------------------------*/
uint8_t getRssiOffset() {
	return RX_64_RSSI_OFFSET;
}
/*---------------------------------------------------------------------------*/
// copy common fields from xbee response to target response
void setCommon(XBeeResponse target) { //&
	target.setApiId(getApiId());
	target.setAvailable(isAvailable());
	target.setChecksum(getChecksum());
	target.setErrorCode(getErrorCode());
	target.setFrameLength(getFrameDataLength());
	target.setMsbLength(getMsbLength());
	target.setLsbLength(getLsbLength());
}
/*---------------------------------------------------------------------------*/
void setFrameData(uint8_t* frameDataPtr) {
	_frameDataPtr = frameDataPtr;
}
/*---------------------------------------------------------------------------*/
uint8_t* getFrameData() {
	return _frameDataPtr;
}
/*---------------------------------------------------------------------------*/
void init() {
	_complete = False;
	_errorCode = NO_ERROR;
	_checksum = 0;
}
/*---------------------------------------------------------------------------*/
void reset() {
	init();
	_apiId = 0;
	_msbLength = 0;
	_lsbLength = 0;
	_checksum = 0;
	_frameLength = 0;

	_errorCode = NO_ERROR;

	for (int i = 0; i < MAX_FRAME_DATA_SIZE; i++) {
		getFrameData()[i] = 0;
	}
}
/*---------------------------------------------------------------------------*/
void getRx16IoSampleResponse(XBeeResponse *response) { //&
	Rx16IoSampleResponse* rx = (Rx16IoSampleResponse*)(&response);

	rx->setFrameData(getFrameData());
	setCommon(response);
}
/*---------------------------------------------------------------------------*/
void Rx64IoSampleResponse() {
    RxIoSampleBaseResponse();
	_remoteAddress = XBeeAddress64();
}
/*---------------------------------------------------------------------------*/
/*XBeeAddress64* getRemoteAddress64() { //&
	return _remoteAddress;
}*/
/*---------------------------------------------------------------------------*/
XBeeAddress64 getRemoteAddress64() {
	return _remoteAddress;
}
/*---------------------------------------------------------------------------*/
uint8_t getStatus() {
    uint8_t* buffer = getFrameData();
	return buffer[1];
}
/*---------------------------------------------------------------------------*/
int isSuccess() {
	return getStatus() == SUCCESS;
}
/*---------------------------------------------------------------------------*/
void getTxStatusResponse(XBeeResponse txResponse) {

	TxStatusResponse* txStatus = (TxStatusResponse*)(&txResponse);

	// pass pointer array to subclass
	txStatus->setFrameData(getFrameData());
	setCommon(txResponse);
}
/*---------------------------------------------------------------------------*/