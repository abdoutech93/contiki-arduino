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

#ifndef XBEE_ADDRESS_H
#define XBEE_ADDRESS_H

#include "xbee_const.h"
/*---------------------------------------------------------------------------*/
/**
 * The super class of all XBee responses (RX packets)
 * Users should never attempt to create an instance of this class; instead
 * create an instance of a subclass
 * It is recommend to reuse subclasses to conserve memory
 */

/*---------------------------------------------------------------------------*/
/**
 * Represents a 64-bit XBee Address
 */
typedef struct XBeeAddress64{
    void (*XBeeAddress) (void);
	void (*XBeeAddress64) (uint32_t msb, uint32_t lsb);
	uint32_t (*getMsb) (void);
	uint32_t (*getLsb) (void);
	void (*setMsb) (uint32_t msb);
	void (*setLsb) (uint32_t lsb);
}XBeeAddress64;
    static uint32_t _msb;
	static uint32_t _lsb;
/*---------------------------------------------------------------------------*/
//class XBeeAddress16 : public XBeeAddress {
//public:
//	XBeeAddress16(uint16_t addr);
//	XBeeAddress16();
//	uint16_t getAddress();
//	void setAddress(uint16_t addr);
//private:
//	uint16_t _addr;
//};
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
#endif //XBee_h