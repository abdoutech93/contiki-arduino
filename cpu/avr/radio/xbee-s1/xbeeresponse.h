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

#ifndef XBEE_RESPONSE_H
#define XBEE_RESPONSE_H

#include "xbee_const.h"
#include "xbeeaddress.h"
/*---------------------------------------------------------------------------*/
typedef struct XBeeResponse XBeeResponse;
struct XBeeResponse {
	//static const int MODEM_STATUS = 0x8a;
	/**
	 * Returns Api Id of the response
	 */
	uint8_t (*getApiId) (void);
	void (*setApiId) (uint8_t apiId);
	/**
	 * Returns the MSB length of the packet
	 */
	uint8_t (*getMsbLength) (void);
	void (*setMsbLength) (uint8_t msbLength);
	/**
	 * Returns the LSB length of the packet
	 */
	uint8_t (*getLsbLength) (void);
	void (*setLsbLength) (uint8_t lsbLength);
	/**
	 * Returns the packet checksum
	 */
	uint8_t (*getChecksum) (void);
	void (*setChecksum) (uint8_t checksum);
	/**
	 * Returns the length of the frame data: all bytes after the api id, and prior to the checksum
	 * Note up to release 0.1.2, this was incorrectly including the checksum in the length.
	 */
	uint8_t (*getFrameDataLength) (void);
	void (*setFrameData) (uint8_t* frameDataPtr);
	/**
	 * Returns the buffer that contains the response.
	 * Starts with byte that follows API ID and includes all bytes prior to the checksum
	 * Length is specified by getFrameDataLength()
	 * Note: Unlike Digi's definition of the frame data, this does not start with the API ID..
	 * The reason for this is all responses include an API ID, whereas my frame data
	 * includes only the API specific data.
	 */
	uint8_t* (*getFrameData) (void);

	void (*setFrameLength) (uint8_t frameLength);
	// to support future 65535 byte packets I guess
	/**
	 * Returns the length of the packet
	 */
	uint16_t (*getPacketLength) (void);
	/**
	 * Resets the response to default values
	 */
	void (*reset) (void);
	/**
	 * Initializes the response
	 */
	void (*init) (void);


	/**
	 * Call with instance of TxStatusResponse only if getApiId() == TX_STATUS_RESPONSE
	 */
	void (*getTxStatusResponse) (XBeeResponse response);//void (*getTxStatusResponse) (XBeeResponse &response);
	/**
	 * Call with instance of Rx16Response only if getApiId() == RX_16_RESPONSE
	 */
	void (*getRx16Response) (XBeeResponse response); //void (*getRx16Response) (XBeeResponse &response);
	/**
	 * Call with instance of Rx64Response only if getApiId() == RX_64_RESPONSE
	 */
	void (*getRx64Response) (XBeeResponse response); //void (*getRx64Response) (XBeeResponse &response);
	/**
	 * Call with instance of Rx16IoSampleResponse only if getApiId() == RX_16_IO_RESPONSE
	 */
	void (*getRx16IoSampleResponse) (XBeeResponse response);//void (*getRx16IoSampleResponse) (XBeeResponse &response);
	/**
	 * Call with instance of Rx64IoSampleResponse only if getApiId() == RX_64_IO_RESPONSE
	 */
	void (*getRx64IoSampleResponse) (XBeeResponse response);//void (*getRx64IoSampleResponse) (XBeeResponse &response);
	/**
	 * Call with instance of AtCommandResponse only if getApiId() == AT_COMMAND_RESPONSE
	 */
	void (*getAtCommandResponse) (XBeeResponse responses); //void (*getAtCommandResponse) (XBeeResponse &responses);
	/**
	 * Call with instance of RemoteAtCommandResponse only if getApiId() == REMOTE_AT_COMMAND_RESPONSE
	 */
	void (*getRemoteAtCommandResponse) (XBeeResponse response); //void (*getRemoteAtCommandResponse) (XBeeResponse &response);
	/**
	 * Call with instance of ModemStatusResponse only if getApiId() == MODEM_STATUS_RESPONSE
	 */
	void (*getModemStatusResponse) (XBeeResponse response);//void (*getModemStatusResponse) (XBeeResponse &response);
	/**
	 * Returns true if the response has been successfully parsed and is complete and ready for use
	 */
	int (*isAvailable) (void);
	void (*setAvailable) (int complete);
	/**
	 * Returns true if the response contains errors
	 */
	int (*isError) (void);
	/**
	 * Returns an error code, or zero, if successful.
	 * Error codes include: CHECKSUM_FAILURE, PACKET_EXCEEDS_BYTE_ARRAY_LENGTH, UNEXPECTED_START_BYTE
	 */
	uint8_t (*getErrorCode) (void);
	void (*setErrorCode) (uint8_t errorCode);
};
// pointer to frameData
	static uint8_t* _frameDataPtr;
    static void setCommon(XBeeResponse target); //    static void setCommon(XBeeResponse &target);
	static uint8_t _apiId;
	static uint8_t _msbLength;
	static uint8_t _lsbLength;
	static uint8_t _checksum;
	static uint8_t _frameLength;
	static int _complete;
	static uint8_t _errorCode;
/*---------------------------------------------------------------------------*/
/**
 * This class is extended by all Responses that include a frame id
 */
typedef struct FrameIdResponse {
	uint8_t (*getFrameId) (void);
    struct XBeeResponse xBeeResponse;
}FrameIdResponse;
	static uint8_t _frameId;
/*---------------------------------------------------------------------------*/
/**
 * Common functionality for both Series 1 and 2 data RX data packets
 */
typedef struct RxDataResponse{
	/**
	 * Returns the specified index of the payload.  The index may be 0 to getDataLength() - 1
	 * This method is deprecated; use uint8_t* getData()
	 */
	uint8_t (*getDataFrom) (int index);
	/**
	 * Returns the payload array.  This may be accessed from index 0 to getDataLength() - 1
	 */
	uint8_t* (*getData) (void);
	/**
	 * Returns the length of the payload
	 */
	uint8_t (*getDataLength) (void);
	/**
	 * Returns the position in the frame data where the data begins
	 */
	uint8_t (*getDataOffset) (void);
    struct XBeeResponse xBeeResponse;
}RxDataResponse;
/*---------------------------------------------------------------------------*/
// getResponse to return the proper subclass:
// we maintain a pointer to each type of response, when a response is parsed, it is allocated only if NULL
// can we allocate an object in a function?

/**
 * Represents a Series 1 TX Status packet
 */
typedef struct TxStatusResponse{
		void (*TxStatusResponse) (void);
		uint8_t (*getStatus) (void);
		int (*isSuccess) (void);
        struct FrameIdResponse frameIdResponse;
}TxStatusResponse;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Series 1 RX packet
 */
typedef struct RxResponse{
	void (*RxResponse) (void);
	// remember rssi is negative but this is unsigned byte so it's up to you to convert
	uint8_t (*getRssi) (void);
	uint8_t (*getOption) (void);
	int (*isAddressBroadcast) (void);
	int (*isPanBroadcast) (void);
	uint8_t (*getDataLength) (void);
	uint8_t (*getDataOffset) (void);
	uint8_t (*getRssiOffset) (void);
    struct RxDataResponse rxDataResponse;
}RxResponse;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Series 1 16-bit address RX packet
 */
typedef struct Rx16Response{
	void (*Rx16Response) (void);
	uint8_t (*getRssiOffset) (void);
	uint16_t (*getRemoteAddress16) (void);
    struct RxResponse rxResponse;
    uint16_t _remoteAddress;//static uint16_t _remoteAddress;
}Rx16Response;
    
/*---------------------------------------------------------------------------*/
/**
 * Represents a Series 1 64-bit address RX packet
 */
typedef struct Rx64Response{
	void (*Rx64Response) (void);
	uint8_t (*getRssiOffset) (void);
	XBeeAddress64 (*getRemoteAddress64) (void);
    struct RxResponse rxResponse;
}Rx64Response;
static XBeeAddress64 _remoteAddress;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Series 1 RX I/O Sample packet
 */
typedef struct RxIoSampleBaseResponse{
		void (*RxIoSampleBaseResponse) (void);
		/**
		 * Returns the number of samples in this packet
		 */
		uint8_t (*getSampleSize) (void);
		int (*containsAnalog) (void);
		int (*containsDigital) (void);
		/**
		 * Returns true if the specified analog pin is enabled
		 */
		int (*isAnalogEnabled) (uint8_t pin);
		/**
		 * Returns true if the specified digital pin is enabled
		 */
		int (*isDigitalEnabled) (uint8_t pin);
		/**
		 * Returns the 10-bit analog reading of the specified pin.
		 * Valid pins include ADC:0-5.  Sample index starts at 0
		 */
		uint16_t (*getAnalog) (uint8_t pin, uint8_t sample);
		/**
		 * Returns true if the specified pin is high/on.
		 * Valid pins include DIO:0-8.  Sample index starts at 0
		 */
		int (*isDigitalOn) (uint8_t pin, uint8_t sample);
		uint8_t (*getSampleOffset) (void);
        struct RxResponse rxResponse;
}RxIoSampleBaseResponse;
/*---------------------------------------------------------------------------*/
typedef struct Rx16IoSampleResponse{
	void (*Rx16IoSampleResponse) (void);
	uint16_t (*getRemoteAddress16) (void);
	uint8_t (*getRssiOffset) (void);
    struct RxIoSampleBaseResponse rxIoSampleBaseResponse;
}Rx16IoSampleResponse;
/*---------------------------------------------------------------------------*/
typedef struct Rx64IoSampleResponse{
    void (*Rx64IoSampleResponse) (void);
	XBeeAddress64 (*getRemoteAddress64) (void); //&
	uint8_t (*getRssiOffset) (void);
    struct RxIoSampleBaseResponse rxIoSampleBaseResponse;
}Rx64IoSampleResponse;
static XBeeAddress64 _remoteAddress;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Modem Status RX packet
 */
typedef struct ModemStatusResponse {
	void (*ModemStatusResponse) (void);
	uint8_t (*getStatus) (void);
    struct XBeeResponse xBeeResponse;
}ModemStatusResponse;
/*---------------------------------------------------------------------------*/
#endif //XBee_h