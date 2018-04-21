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

#ifndef ATCOMMAND_RESPONSE_H
#define ATCOMMAND_RESPONSE_H

#include "xbee_const.h"
#include "xbeeresponse.h"
/*---------------------------------------------------------------------------*/
/**
 * Represents an AT Command RX packet
 */
typedef struct AtCommandResponse {
		/**
		 * Returns an array containing the two character command
		 */
		uint8_t* (*getCommand) (void);
		/**
		 * Returns the command status code.
		 * Zero represents a successful command
		 */
		uint8_t (*getStatus) (void);
		/**
		 * Returns an array containing the command value.
		 * This is only applicable to query commands.
		 */
		uint8_t* (*getValue) (void);
		/**
		 * Returns the length of the command value array.
		 */
		uint8_t (*getValueLength) (void);
		/**
		 * Returns true if status equals AT_OK
		 */
		int (*isOk) (void);
        struct FrameIdResponse;
}AtCommandResponse;
/*---------------------------------------------------------------------------*/
/**
 * Represents a Remote AT Command RX packet
 */
typedef struct RemoteAtCommandResponse {
        
		void (*RemoteAtCommandResponse) (void);
		/**
		 * Returns an array containing the two character command
		 */
		uint8_t* (*getCommand) (void);
		/**
		 * Returns the command status code.
		 * Zero represents a successful command
		 */
		uint8_t (*getStatus) (void);
		/**
		 * Returns an array containing the command value.
		 * This is only applicable to query commands.
		 */
		uint8_t* (*getValue) (void);
		/**
		 * Returns the length of the command value array.
		 */
		uint8_t (*getValueLength) (void);
		/**
		 * Returns the 16-bit address of the remote radio
		 */
		uint16_t (*getRemoteAddress16) (void);
		/**
		 * Returns the 64-bit address of the remote radio
		 */
		XBeeAddress64* (*getRemoteAddress64) (void); //&
		/**
		 * Returns true if command was successful
		 */
		int (*isOk) (void);
		struct AtCommandResponse;
}RemoteAtCommandResponse;
static XBeeAddress64 _remoteAddress64;

#endif //XBee_h