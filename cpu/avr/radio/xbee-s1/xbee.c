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
#include "Arduino.h"
#include "contiki.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "xbee.h"
#include "dev/radio.h"
#include "net/packetbuf.h"
#include "net/netstack.h"

#include "xbee_const.h"
#include "xbeeresponse.h"
#include "xbeerequest.h"
#include "xbeeaddress.h"
#include "atcommandresponse.h"
#include "atcommandrequest.h"
/*---------------------------------------------------------------------------*/
PROCESS(xbee_process, "Xbee Driver");
static uint8_t receive_on;
static uint8_t locked, lock_on, lock_off;
static int channel;
static volatile uint16_t last_packet_timestamp;
/*---------------------------------------------------------------------------*/
#define GET_LOCK() locked++
static void RELEASE_LOCK(void) {
  if(locked == 1) {
    if(lock_on) {
      on();
      lock_on = 0;
    }
    if(lock_off) {
      off();
      lock_off = 0;
    }
  }
  locked--;
}
/*---------------------------------------------------------------------------*/
int
xbee_init(void)
{
  

  flushrx();

  process_start(&xbee_process, NULL);
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
xbee_transmit(unsigned short payload_len)
{
  int i, txpower;

  GET_LOCK();

  txpower = 0;
  

      RELEASE_LOCK();

      return RADIO_TX_OK;
    
  

  /* If we are using WITH_SEND_CCA, we get here if the packet wasn't
     transmitted because of other channel activity. */
  //RIMESTATS_ADD(contentiondrop);
  PRINTF("xbee: do_send() transmission never started\n");

  if(packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER) > 0) {
    /* Restore the transmission power */
    set_txpower(txpower & 0xff);
  }

  RELEASE_LOCK();
  return RADIO_TX_COLLISION;
}
/*---------------------------------------------------------------------------*/
static int
xbee_prepare(const void *payload, unsigned short payload_len)
{
  uint8_t total_len;
  GET_LOCK();

  PRINTF("xbee: sending %d bytes\n", payload_len);
  

  RELEASE_LOCK();
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
xbee_send(const void *payload, unsigned short payload_len)
{
  xbee_prepare(payload, payload_len);
  return xbee_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
int
xbee_off(void)
{
  /* Don't do anything if we are already turned off. */
  if(receive_on == 0) {
    return 1;
  }

  /* If we are called when the driver is locked, we indicate that the
     radio should be turned off when the lock is unlocked. */
  if(locked) {
    /*    printf("Off when locked (%d)\n", locked);*/
    lock_off = 1;
    return 1;
  }

  GET_LOCK();
  /* If we are currently receiving a packet (indicated by SFD == 1),
     we don't actually switch the radio off now, but signal that the
     driver should switch off the radio once the packet has been
     received and processed, by setting the 'lock_off' variable. */
  if(status() /*& BV(xbee_TX_ACTIVE)*/) {
    lock_off = 1;
  } else {
    off();
  }
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
int
xbee_on(void)
{
  if(receive_on) {
    return 1;
  }
  if(locked) {
    lock_on = 1;
    return 1;
  }

  GET_LOCK();
  on();
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
int
xbee_get_channel(void)
{
  return channel;
}
/*---------------------------------------------------------------------------*/
int
xbee_set_channel(int c)
{
  uint16_t f;

  GET_LOCK();
  
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
void
xbee_set_pan_addr(unsigned pan,
                    unsigned addr,
                    const uint8_t *ieee_addr)
{
  uint8_t tmp[2];

  GET_LOCK();

  /*
   * Writing RAM requires crystal oscillator to be stable.
   */
  
  RELEASE_LOCK();
}
/*---------------------------------------------------------------------------*/
/*
 * Interrupt leaves frame intact in FIFO.
 */

xbee_interrupt(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(xbee_process, ev, data)
{
  int len;
  PROCESS_BEGIN();

  PRINTF("xbee_process: started\n");

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
#if XBEE_TIMETABLE_PROFILING
    TIMETABLE_TIMESTAMP(xbee_timetable, "poll");
#endif /* xbee_TIMETABLE_PROFILING */

    PRINTF("xbee_process: calling receiver callback\n");

    packetbuf_clear();
    packetbuf_set_attr(PACKETBUF_ATTR_TIMESTAMP, last_packet_timestamp);
    len = xbee_read(packetbuf_dataptr(), PACKETBUF_SIZE);
    packetbuf_set_datalen(len);

    NETSTACK_RDC.input();
    /* flushrx(); */
#if XBEE_TIMETABLE_PROFILING
    TIMETABLE_TIMESTAMP(xbee_timetable, "end");
    timetable_aggregate_compute_detailed(&aggregate_time,
                                         &xbee_timetable);
    timetable_clear(&xbee_timetable);
#endif /* xbee_TIMETABLE_PROFILING */
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static int
xbee_read(void *buf, unsigned short bufsize)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
void
xbee_set_txpower(uint8_t power)
{
  GET_LOCK();
  set_txpower(power);
  RELEASE_LOCK();
}
/*---------------------------------------------------------------------------*/
int
xbee_get_txpower(void)
{
  uint8_t power;
  GET_LOCK();
  //power = getreg(xbee_TXPOWER);
  RELEASE_LOCK();
  return power;
}
/*---------------------------------------------------------------------------*/
int
xbee_rssi(void)
{
  int rssi;
  int radio_was_off = 0;

  if(locked) {
    return 0;
  }

  GET_LOCK();

  if(!receive_on) {
    radio_was_off = 1;
    xbee_on();
  }
  //BUSYWAIT_UNTIL(status() & BV(xbee_RSSI_VALID), RTIMER_SECOND / 100);

  //rssi = (int)((signed char)getreg(xbee_RSSI));

  if(radio_was_off) {
    xbee_off();
  }
  RELEASE_LOCK();
  return rssi;
}
/*---------------------------------------------------------------------------*/
/*
static int
detected_energy(void)
{
  return xbee_rssi();
}
*/
/*---------------------------------------------------------------------------*/
int
xbee_cca_valid(void)
{
  int valid;
  if(locked) {
    return 1;
  }
  GET_LOCK();
  //valid = !!(status() & BV(xbee_RSSI_VALID));
  RELEASE_LOCK();
  return valid;
}
/*---------------------------------------------------------------------------*/
static int
xbee_cca(void)
{
  int cca;
  int radio_was_off = 0;

  /* If the radio is locked by an underlying thread (because we are
     being invoked through an interrupt), we preted that the coast is
     clear (i.e., no packet is currently being transmitted by a
     neighbor). */
  if(locked) {
    return 1;
  }

  GET_LOCK();
  if(!receive_on) {
    radio_was_off = 1;
    xbee_on();
  }

  /* Make sure that the radio really got turned on. */
  if(!receive_on) {
    RELEASE_LOCK();
    if(radio_was_off) {
      xbee_off();
    }
    return 1;
  }

  //BUSYWAIT_UNTIL(status() & BV(xbee_RSSI_VALID), RTIMER_SECOND / 100);

  //cca = xbee_CCA_IS_1;

  if(radio_was_off) {
    xbee_off();
  }
  RELEASE_LOCK();
  return cca;
}
/*---------------------------------------------------------------------------*/
int
xbee_receiving_packet(void)
{
  return;
}
/*---------------------------------------------------------------------------*/
static int
pending_packet(void)
{
  return;
}
/*---------------------------------------------------------------------------*/
void
xbee_set_cca_threshold(int value)
{
  GET_LOCK();
  
  RELEASE_LOCK();
}
/*---------------------------------------------------------------------------*/
const struct radio_driver xbee_driver =
  {
    xbee_init,
    xbee_prepare,
    xbee_transmit,
    xbee_send,
    xbee_read,
    /* xbee_set_channel, */
    /* detected_energy, */
    xbee_cca,
    xbee_receiving_packet,
    pending_packet,
    xbee_on,
    xbee_off,
  };
/*---------------------------------------------------------------------------*/
uint8_t getSampleOffset() {
	// sample starts 2 bytes after rssi
	return getRssiOffset() + 2;
}
/*---------------------------------------------------------------------------*/
uint8_t getSampleSize() {
	uint8_t* buffer = getFrameData();
    return buffer[getSampleOffset()];
}
/*---------------------------------------------------------------------------*/
int containsAnalog() {
    uint8_t* buffer=getFrameData();
	return (buffer[getSampleOffset() + 1] & 0x7e) > 0;
}
/*---------------------------------------------------------------------------*/
int containsDigital() {
    uint8_t* buffer=getFrameData();
	return (buffer[getSampleOffset() + 1] & 0x1) > 0 || buffer[getSampleOffset() + 2] > 0;
}
/*---------------------------------------------------------------------------*/
//uint16_t RxIoSampleBaseResponse::getAnalog0(uint8_t sample) {
//	return getAnalog(0, sample);
//}
/*---------------------------------------------------------------------------*/
int isAnalogEnabled(uint8_t pin) {
    uint8_t* buffer=getFrameData();
	return (((buffer[getSampleOffset() + 1] >> (pin + 1)) & 1) == 1);
}
/*---------------------------------------------------------------------------*/
int isDigitalEnabled(uint8_t pin) {
	uint8_t* buffer;
    if (pin < 8) {
        buffer=getFrameData();
		return ((buffer[getSampleOffset() + 4] >> pin) & 1) == 1;
	} else {
        buffer=getFrameData();
		return (buffer[getSampleOffset() + 3] & 1) == 1;
	}
}
/*---------------------------------------------------------------------------*/
//	// verified (from XBee-API)
//	private int getSampleWidth() {
//		int width = 0;
//
//		// width of sample depends on how many I/O pins are enabled. add one for each analog that's enabled
//		for (int i = 0; i <= 5; i++) {
//			if (isAnalogEnabled(i)) {
//				// each analog is two bytes
//				width+=2;
//			}
//		}
//		
//		if (this.containsDigital()) {
//			// digital enabled takes two bytes, no matter how many pins enabled
//			width+= 2;
//		}
//		
//		return width;
//	}
//
//	private int getStartIndex() {
//
//		int startIndex;
//
//		if (this.getSourceAddress() instanceof XBeeAddress16) {
//			// 16 bit
//			startIndex = 7;
//		} else {
//			// 64 bit
//			startIndex = 13;
//		}
//		
//		return startIndex;
//	}
//	
//	public int getDigitalMsb(int sample) {
//		// msb digital always starts 3 bytes after sample size
//		return this.getProcessedPacketBytes()[this.getStartIndex() + 3 + this.getSampleWidth() * sample];
//	}
//	
//	public int getDigitalLsb(int sample) {
//		return this.getProcessedPacketBytes()[this.getStartIndex() + 3 + this.getSampleWidth() * sample + 1];
//	}	
//
//	public Boolean isDigitalOn(int pin, int sample) {
//		
//		if (sample < 0 || sample >= this.getSampleSize()) {
//			throw new IllegalArgumentException("invalid sample size: " + sample);
//		}
//		
//		if (!this.containsDigital()) {
//			throw new RuntimeException("Digital is not enabled");
//		}
//		
//		if (pin >= 0 && pin < 8) {
//			return ((this.getDigitalLsb(sample) >> pin) & 1) == 1;
//		} else if (pin == 8) {
//			// uses msb dio line
//			return (this.getDigitalMsb(sample) & 1) == 1;
//		} else {
//			throw new IllegalArgumentException("Invalid pin: " + pin);
//		}		
//	}
//	
//	public Integer getAnalog(int pin, int sample) {
//		
//		if (sample < 0 || sample >= this.getSampleSize()) {
//			throw new IllegalArgumentException("invalid sample size: " + sample);
//		}
//		
//		// analog starts 3 bytes after start of sample, if no dio enabled
//		int startIndex = this.getStartIndex() + 3;
//		
//		if (this.containsDigital()) {
//			// make room for digital i/o sample (2 bytes per sample)
//			startIndex+= 2;
//		}
//		
//		startIndex+= this.getSampleWidth() * sample;
//
//		// start depends on how many pins before this pin are enabled
//		// this will throw illegalargumentexception if invalid pin
//		for (int i = 0; i < pin; i++) {
//			if (isAnalogEnabled(i)) {
//				startIndex+=2;
//			}
//		}
//
//		return (this.getProcessedPacketBytes()[startIndex] << 8) + this.getProcessedPacketBytes()[startIndex + 1];		
//	}				
// THIS IS WRONG
/*---------------------------------------------------------------------------*/
uint16_t getAnalog(uint8_t pin, uint8_t sample) {

	// analog starts 3 bytes after sample size, if no dio enabled
	uint8_t start = 3;

	if (containsDigital()) {
		// make room for digital i/o sample (2 bytes per sample)
		start+=2*(sample + 1);
	}

	uint8_t spacing = 0;
    int i;
	// spacing between samples depends on how many are enabled. add one for each analog that's enabled
	for (i = 0; i <= 5; i++) {
		if (isAnalogEnabled(i)) {
			// each analog is two bytes
			spacing+=2;
		}
	}

//	std::cout << "spacing is " << static_cast<unsigned int>(spacing) << std::endl;

	// start depends on how many pins before this pin are enabled
	for (i = 0; i < pin; i++) {
		if (isAnalogEnabled(i)) {
			start+=2;
		}
	}

	start+= sample * spacing;

//	std::cout << "start for analog pin ["<< static_cast<unsigned int>(pin) << "]/sample " << static_cast<unsigned int>(sample) << " is " << static_cast<unsigned int>(start) << std::endl;

//	std::cout << "returning index " << static_cast<unsigned int>(getSampleOffset() + start) << " and index " <<  static_cast<unsigned int>(getSampleOffset() + start + 1) << ", val is " << static_cast<unsigned int>(getFrameData()[getSampleOffset() + start] << 8) <<  " and " <<  + static_cast<unsigned int>(getFrameData()[getSampleOffset() + start + 1]) << std::endl;
    uint8_t* buffer = getFrameData();
	return (uint16_t)((buffer[getSampleOffset() + start] << 8) + buffer[getSampleOffset() + start + 1]);
}
/*---------------------------------------------------------------------------*/
int isDigitalOn(uint8_t pin, uint8_t sample) {
    uint8_t* buffer;
	if (pin < 8) {
        buffer = getFrameData();
		return ((buffer[getSampleOffset() + 4] >> pin) & 1) == 1;
	} else {
        buffer = getFrameData();
		return (buffer[getSampleOffset() + 3] & 1) == 1;
	}
}
/*---------------------------------------------------------------------------*/
//int RxIoSampleBaseResponse::isDigital0On(uint8_t sample) {
//	return isDigitalOn(0, sample);
//}
/*---------------------------------------------------------------------------*/
void getRx64IoSampleResponse(XBeeResponse response) {
	Rx64IoSampleResponse* rx = (Rx64IoSampleResponse*)(&response);
	response.setFrameData(getFrameData());
	setCommon(response);
    uint8_t* buffer = getFrameData();
	rx->getRemoteAddress64().setMsb(((uint32_t)(buffer[0]) << 24) + ((uint32_t)(buffer[1]) << 16) + ((uint16_t)(buffer[2]) << 8) + buffer[3]);
	rx->getRemoteAddress64().setLsb(((uint32_t)(buffer[4]) << 24) + ((uint32_t)(buffer[5]) << 16) + ((uint16_t)(buffer[6]) << 8) + buffer[7]);
}
/*---------------------------------------------------------------------------*/
uint8_t getRssi() {
    uint8_t* buffer = getFrameData();
	return buffer[getRssiOffset()];
}
/*---------------------------------------------------------------------------*/
uint8_t getOption() {
    uint8_t* buffer = getFrameData();
	return buffer[getRssiOffset() + 1];
}
/*---------------------------------------------------------------------------*/
int isAddressBroadcast() {
	return (getOption() & 2) == 2;
}
/*---------------------------------------------------------------------------*/
int isPanBroadcast() {
	return (getOption() & 4) == 4;
}
/*---------------------------------------------------------------------------*/
void getRx16Response(XBeeResponse rx16Response) {

	Rx16Response* rx16 = (Rx16Response*) (&rx16Response);

	// pass pointer array to subclass
	rx16Response.setFrameData(getFrameData());
	setCommon(rx16Response);

//	rx16->getRemoteAddress16().setAddress((getFrameData()[0] << 8) + getFrameData()[1]);
}
/*---------------------------------------------------------------------------*/
void getRx64Response(XBeeResponse rx64Response) {

	Rx64Response* rx64 = (Rx64Response*) (&rx64Response);

	// pass pointer array to subclass
	rx64Response.setFrameData(getFrameData());
	setCommon(rx64Response);
    uint8_t* buffer = getFrameData();
	rx64->getRemoteAddress64().setMsb(((uint32_t)(buffer[0]) << 24) + ((uint32_t)(buffer[1]) << 16) + ((uint16_t)(buffer[2]) << 8) + buffer[3]);
	rx64->getRemoteAddress64().setLsb(((uint32_t)(buffer[4]) << 24) + ((uint32_t)(buffer[5]) << 16) + ((uint16_t)(buffer[6]) << 8) + buffer[7]);
}
/*---------------------------------------------------------------------------*/
void getModemStatusResponse(XBeeResponse modemStatusResponse) {

	ModemStatusResponse* modem = (ModemStatusResponse*) (&modemStatusResponse);

	// pass pointer array to subclass
	modemStatusResponse.setFrameData(getFrameData());
	setCommon(modemStatusResponse);

}
/*---------------------------------------------------------------------------*/
uint8_t* getCommand() {
	return getFrameData() + 1;
}
/*---------------------------------------------------------------------------*/
uint8_t getStatus() {
    uint8_t* buffer = getFrameData();
	return buffer[3];
}
/*---------------------------------------------------------------------------*/
uint8_t getValueLength() {
	return getFrameDataLength() - 4;
}
/*---------------------------------------------------------------------------*/
uint8_t* getValue() {
	if (getValueLength() > 0) {
		// value is only included for query commands.  set commands does not return a value
		return getFrameData() + 4;
	}

	return NULL;
}
/*---------------------------------------------------------------------------*/
int isOk() {
	return getStatus() == AT_OK;
}
/*---------------------------------------------------------------------------*/
void getAtCommandResponse(XBeeResponse atCommandResponse) {

	//AtCommandResponse* at = (AtCommandResponse*) (&atCommandResponse);

	// pass pointer array to subclass
	atCommandResponse.setFrameData(getFrameData());
	setCommon(atCommandResponse);
}
/*---------------------------------------------------------------------------*/
uint16_t getPacketLength() {
	return ((_msbLength << 8) & 0xff) + (_lsbLength & 0xff);
}
/*---------------------------------------------------------------------------*/
void resetResponse() {
	_pos = 0;
	_escape = FALSE;
	_response.reset();
}
/*---------------------------------------------------------------------------*/
void XBeeOpen(void) {
	_pos = 0;
	_escape = FALSE;
	_checksumTotal = 0;
	_nextFrameId = 0;
	
	_response.init();
	_response.setFrameData(_responseFrameData);
	// default
	_serial = &Serial;
}
/*---------------------------------------------------------------------------*/
uint8_t getNextFrameId() {

	_nextFrameId++;

	if (_nextFrameId == 0) {
		// can't send 0 because that disables status response
		_nextFrameId = 1;
	}

	return _nextFrameId;
}
/*---------------------------------------------------------------------------*/
void begin(long baud) {
	_serial->begin(baud);
}
/*---------------------------------------------------------------------------*/
void setSerial(HardwareSerial serial) {
	_serial = &serial;
}
/*---------------------------------------------------------------------------*/
int available() {
	return _serial->available();
}
/*---------------------------------------------------------------------------*/
uint8_t read() {
	return _serial->read();
} 
/*---------------------------------------------------------------------------*/
void flush() {
	_serial->flush();
} 
/*---------------------------------------------------------------------------*/
void write(uint8_t val) {
	_serial->write(val);
}
/*---------------------------------------------------------------------------*/
XBeeResponse getResponse() {
	return _response;
}
/*---------------------------------------------------------------------------*/
// TODO how to convert response to proper subclass?
void getResponseIn(XBeeResponse response) {

	response.setMsbLength(_response.getMsbLength());
	response.setLsbLength(_response.getLsbLength());
	response.setApiId(_response.getApiId());
	response.setFrameLength(_response.getFrameDataLength());

	response.setFrameData(_response.getFrameData());
}
/*---------------------------------------------------------------------------*/
void readPacketUntilAvailable() {
	while (!(getResponse().isAvailable() || getResponse().isError())) {
		// read some more
		waitforPacket();//readPacket();
	}
}
/*---------------------------------------------------------------------------*/
int readPacket(int timeout) {

	if (timeout < 0) {
		return FALSE;
	}

	unsigned long start = millis();

    while (((int)((millis() - start))) < timeout) {

     	waitforPacket();//readPacket();

     	if (getResponse().isAvailable()) {
     		return TRUE;
     	} else if (getResponse().isError()) {
     		return FALSE;
     	}
    }

    // timed out
    return FALSE;
}
/*---------------------------------------------------------------------------*/
//void readPacket()
void waitforPacket() {
	// reset previous response
	if (_response.isAvailable() || _response.isError()) {
		// discard previous packet and start over
		resetResponse();
	}

    while (available()) {

        b = read();

        if (_pos > 0 && b == START_BYTE && ATAP == 2) {
        	// new packet start before previous packeted completed -- discard previous packet and start over
        	_response.setErrorCode(UNEXPECTED_START_BYTE);
        	return;
        }

		if (_pos > 0 && b == ESCAPE) {
			if (available()) {
				b = read();
				b = 0x20 ^ b;
			} else {
				// escape byte.  next byte will be
				_escape = TRUE;
				continue;
			}
		}

		if (_escape == TRUE) {
			b = 0x20 ^ b;
			_escape = FALSE;
		}

		// checksum includes all bytes starting with api id
		if (_pos >= API_ID_INDEX) {
			_checksumTotal+= b;
		}

        switch(_pos) {
			case 0:
		        if (b == START_BYTE) {
		        	_pos++;
		        }

		        break;
			case 1:
				// length msb
				_response.setMsbLength(b);
				_pos++;

				break;
			case 2:
				// length lsb
				_response.setLsbLength(b);
				_pos++;

				break;
			case 3:
				_response.setApiId(b);
				_pos++;

				break;
			default:
				// starts at fifth byte

				if (_pos > MAX_FRAME_DATA_SIZE) {
					// exceed max size.  should never occur
					_response.setErrorCode(PACKET_EXCEEDS_BYTE_ARRAY_LENGTH);
					return;
				}

				// check if we're at the end of the packet
				// packet length does not include start, length, or checksum bytes, so add 3
				if (_pos == (_response.getPacketLength() + 3)) {
					// verify checksum

					//std::cout << "read checksum " << static_cast<unsigned int>(b) << " at pos " << static_cast<unsigned int>(_pos) << std::endl;

					if ((_checksumTotal & 0xff) == 0xff) {
						_response.setChecksum(b);
						_response.setAvailable(TRUE);

						_response.setErrorCode(NO_ERROR);
					} else {
						// checksum failed
						_response.setErrorCode(CHECKSUM_FAILURE);
					}

					// minus 4 because we start after start,msb,lsb,api and up to but not including checksum
					// e.g. if frame was one byte, _pos=4 would be the byte, pos=5 is the checksum, where end stop reading
					_response.setFrameLength(_pos - 4);

					// reset state vars
					_pos = 0;

					_checksumTotal = 0;

					return;
				} else {
                    uint8_t* buffer = _response.getFrameData();
					// add to packet array, starting with the fourth byte of the apiFrame
					buffer[_pos - 4] = b;
					_pos++;
				}
        }
    }
}
/*---------------------------------------------------------------------------*/
//void XBeeRequest::reset() {
//	_frameId = DEFAULT_FRAME_ID;
//}

//uint8_t XBeeRequest::getPayloadOffset() {
//	return _payloadOffset;
//}
//
//uint8_t XBeeRequest::setPayloadOffset(uint8_t payloadOffset) {
//	_payloadOffset = payloadOffset;
//}
/*---------------------------------------------------------------------------*/
// TODO
//GenericRequest::GenericRequest(uint8_t* frame, uint8_t len, uint8_t apiId): XBeeRequest(apiId, *(frame), len) {
//	_frame = frame;
//}

void send(XBeeRequest request) { //&
	// the new new deal

	sendByte(START_BYTE, FALSE);

	// send length
	uint8_t msbLen = ((request.getFrameDataLength() + 2) >> 8) & 0xff;
	uint8_t lsbLen = (request.getFrameDataLength() + 2) & 0xff;

	sendByte(msbLen, TRUE);
	sendByte(lsbLen, TRUE);

	// api id
	sendByte(request.getApiId(), TRUE);
	sendByte(request.getFrameId(), TRUE);

	uint8_t checksum = 0;

	// compute checksum, start at api id
	checksum+= request.getApiId();
	checksum+= request.getFrameId();

	//std::cout << "frame length is " << static_cast<unsigned int>(request.getFrameDataLength()) << std::endl;
    int i;
	for (i = 0; i < request.getFrameDataLength(); i++) {
//		std::cout << "sending byte [" << static_cast<unsigned int>(i) << "] " << std::endl;
		sendByte(request.getFrameData(i), TRUE);
		checksum+= request.getFrameData(i);
	}

	// perform 2s complement
	checksum = 0xff - checksum;

//	std::cout << "checksum is " << static_cast<unsigned int>(checksum) << std::endl;

	// send checksum
	sendByte(checksum, TRUE);

	// send packet (Note: prior to Arduino 1.0 this flushed the incoming buffer, which of course was not so great)
	flush();
}
/*---------------------------------------------------------------------------*/
void sendByte(uint8_t b, int escape) {

	if (escape && (b == START_BYTE || b == ESCAPE || b == XON || b == XOFF)) {
//		std::cout << "escaping byte [" << toHexString(b) << "] " << std::endl;
		write(ESCAPE);
		write(b ^ 0x20);
	} else {
		write(b);
	}
}
/*---------------------------------------------------------------------------*/