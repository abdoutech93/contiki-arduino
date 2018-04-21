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
#include "xbeeaddress.h"
/*---------------------------------------------------------------------------*/
void XBeeAddress64(uint32_t msb, uint32_t lsb){
	_msb = msb;
	_lsb = lsb;
}
/*---------------------------------------------------------------------------*/
uint32_t getMsb() {
	return _msb;
}
/*---------------------------------------------------------------------------*/
void setMsb(uint32_t msb) {
	_msb = msb;
}
/*---------------------------------------------------------------------------*/
uint32_t getLsb() {
	return _lsb;
}
/*---------------------------------------------------------------------------*/
void setLsb(uint32_t lsb) {
	_lsb = lsb;
}
