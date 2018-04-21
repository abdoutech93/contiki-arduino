// DHT Temperature & Humidity Unified Sensor Library
// Copyright (c) 2014 Adafruit Industries
// Author: Tony DiCola

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Arduino.h"
#include <avr/io.h>
#include "contiki.h"
#include "dht.h"
#include "gpio.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/sensors.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
extern const struct sensors_sensor dht;
SENSORS_SENSOR(dht, DHT_SENSOR, value, configure, NULL);
/*---------------------------------------------------------------------------*/
static uint8_t enabled;
static uint8_t busy;
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];
/*---------------------------------------------------------------------------*/
#define DHT_PORT_BASE          GPIO_PORT_TO_BASE(DHT_PORT)
#define DHT_PIN_MASK           digitalPinToBitMask(DHT_PIN)
/*---------------------------------------------------------------------------*/
uint32_t expectPulse(int level) {
  uint32_t count = 0;
  // On AVR platforms use direct GPIO port access as it's much faster and better
  // for catching pulses that are 10's of microseconds in length:
  #ifdef __AVR
    uint8_t portState = level ? _bit : 0;
    while ((*portInputRegister(DHT_PORT) & _bit) == portState) {
      if (count++ >= _maxcycles) {
        return 0; // Exceeded timeout, fail.
      }
    }
  // Otherwise fall back to using digitalRead (this seems to be necessary on ESP8266
  // right now, perhaps bugs in direct port access functions?).
  #else
    while (digitalRead(DHT_PIN) == level) {
      if (count++ >= _maxcycles) {
        return 0; // Exceeded timeout, fail.
      }
    }
  #endif

  return count;
}
/*---------------------------------------------------------------------------*/
int read(int force) {
  // Check if sensor was read less than two seconds ago and return early
  // to use last reading.
  uint32_t currenttime = clock_time();
  if (!force && ((currenttime - _lastreadtime) < CLOCK_SECOND*2)) {
    return _lastresult; // return last correct measurement
  }
  _lastreadtime = currenttime;

  // Reset 40 bits of received data to zero.
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  // Send start signal.  See DHT datasheet for full signal diagram:
  //   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf

  // Go into high impedence state to let pull-up raise data line level and
  // start the reading process.
  digitalWrite(DHT_PIN, HIGH);
  _delay_ms(250);

  // First set data line low for 20 milliseconds.
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  _delay_ms(20);


  // Turn off interrupts temporarily because the next sections are timing critical
    // and we don't want any interruptions.
    uint8_t volatile sreg;
    sreg = SREG;    /* Save status register before disabling interrupts. */
    uint32_t cycles[80];
    int i;
    cli();          /* Disable interrupts. */
    //for (i=0; i<MAXCYCLES; i++) {
    {
    // End the start signal by setting data line high for 40 microseconds.
    digitalWrite(DHT_PIN, HIGH);
    _delay_us(40);//delayMicroseconds(40);

    // Now start reading the data line to get the value from the DHT sensor.
    pinMode(DHT_PIN, INPUT_PULLUP);
    _delay_us(10);//delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

    // First expect a low signal for ~80 microseconds followed by a high signal
    // for ~80 microseconds again.
    if (expectPulse(LOW) == 0) {
      //DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
      _lastresult = DHT_BUSY;
      return _lastresult;
    }
    if (expectPulse(HIGH) == 0) {
      //DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
      _lastresult = DHT_BUSY;
      return _lastresult;
    }

    // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
    // microsecond low pulse followed by a variable length high pulse.  If the
    // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
    // then it's a 1.  We measure the cycle count of the initial 50us low pulse
    // and use that to compare to the cycle count of the high pulse to determine
    // if the bit is a 0 (high state cycle count < low state cycle count), or a
    // 1 (high state cycle count > low state cycle count). Note that for speed all
    // the pulses are read into a array and then examined in a later step.
    for (i=0; i<MAXCYCLES; i+=2) {
      cycles[i]   = expectPulse(LOW);
      cycles[i+1] = expectPulse(HIGH);
    }
  } // Timing critical code is now complete.

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (i=0; i<40; ++i) {
    uint32_t lowCycles  = cycles[2*i];
    uint32_t highCycles = cycles[2*i+1];
    if ((lowCycles == 0) || (highCycles == 0)) {
      //DEBUG_PRINTLN(F("Timeout waiting for pulse."));
      _lastresult = DHT_BUSY;
      return _lastresult;
    }
    data[i/8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      data[i/8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }
    SREG = sreg;    /* Enable interrupts. */
    sei();
  printf("Received:\n");
  printf("%d %d %d %d %d %d \n",data[0],data[1],data[2],data[3],data[4],(uint8_t)(data[0] + data[1] + data[2] + data[3]) );
  /*printf(data[0], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[1], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[2], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[3], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(data[4], HEX); DEBUG_PRINT(F(" =? "));
  DEBUG_PRINTLN((data[0] + data[1] + data[2] + data[3]) & 0xFF, HEX);*/

  // Check we read 40 bits and that the checksum matches.
  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    _lastresult = DHT_SUCCESS;
    return _lastresult;
  }
  else {
    //DEBUG_PRINTLN(F("Checksum failure!"));
    _lastresult = DHT_ERROR;
    return _lastresult;
  }
}
/*---------------------------------------------------------------------------*/
uint16_t convertCtoF(uint16_t c) {
  return c * 1.8 + 32;
}
/*---------------------------------------------------------------------------*/
uint16_t convertFtoC(uint16_t f) {
  return (f - 32) * 0.55555;
}
/*---------------------------------------------------------------------------*/
//boolean S == Scale.  True == Fahrenheit; False == Celcius
static uint16_t readTemperature(int S, int force) {
  uint16_t f;

  if (read(force)==DHT_SUCCESS) {
    switch (DHT_TYPE) {
    case DHT11:
        printf("TMP : %d\n",data[2]);
      f = data[2];
      if(S) {
        f = convertCtoF(f);
      }
      break;
    case DHT22:
    case DHT21:
      f = data[2] & 0x7F;
      f *= 256;
      f += data[3];
      f *= 0.1;
      if (data[2] & 0x80) {
        f *= -1;
      }
      if(S) {
        f = convertCtoF(f);
      }
      break;
    }
  }
  return f;
}
/*---------------------------------------------------------------------------*/
uint16_t readHumidity(int force) {
  uint16_t f;
  if (read(0)==DHT_SUCCESS) {
    switch (DHT_TYPE) {
    case DHT11:
        printf("HUM : %d\n",data[0]);
      f = data[0];
      break;
    case DHT22:
    case DHT21:
      f = data[0];
      f *= 256;
      f += data[1];
      f *= 0.1;
      break;
    }
  }
  return f;
}
/*---------------------------------------------------------------------------*/
uint8_t DHT_Read_Data(uint16_t *temperature, uint16_t *humidity){
    if (read(0)==DHT_SUCCESS){
        *humidity = readHumidity(0);
        *temperature = readTemperature(0,0);
        return DHT_SUCCESS;
    }
    return DHT_ERROR;
}
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  if((type != DHT_READ_HUM) && (type != DHT_READ_TEMP) &&
     (type != DHT_READ_ALL)) {
    PRINTF("DHT: Invalid type %u\n", type);
    return DHT_ERROR;
  }

  if(busy) {
    PRINTF("DHT: ongoing operation, wait\n");
    return DHT_BUSY;
  }

  busy = 1;

  if(read(0) != DHT_SUCCESS) {
    PRINTF("DHT: Fail to read sensor\n");
    GPIO_SET_INPUT(DHT_PORT_BASE, DHT_PIN_MASK);
    GPIO_SET_PIN(DHT_PORT_BASE, DHT_PIN_MASK);
    busy = 0;
    return DHT_ERROR;
  }

  switch(type) {
  case DHT_READ_HUM:
    return readHumidity(0);
  case DHT_READ_TEMP:
    return readTemperature(0,0);
  case DHT_READ_ALL:
    return DHT_SUCCESS;
  default:
    return DHT_ERROR;
  }
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int value)
{
  if(type != SENSORS_ACTIVE) {
    printf("ERROR ACTIVATION\n");
    return DHT_ERROR;
  }
    _bit = DHT_PIN_MASK;
    _port = DHT_PORT;
    _maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for
                                                 // reading pulses from DHT sensor.
  // Note that count is now ignored as the DHT reading algorithm adjusts itself
  // basd on the speed of the processor.
  // set up the pins!
  pinMode(DHT_PIN, INPUT_PULLUP);
  // Using this value makes sure that millis() - lastreadtime will be
  // >= MIN_INTERVAL right away. Note that this assignment wraps around,
  // but so will the subtraction.
  _lastreadtime = -MIN_INTERVAL;
  printf("Max clock cycles: %d\n",_maxcycles);
  printf("Sensor type : DHT%d\n",DHT_TYPE);
  if(value) {
    printf("SENSOR ENABLED\n");
    enabled = 1;
    return DHT_SUCCESS;
  }
  printf("SENSOR NOT ENABLED\n");
  enabled = 0;
  return DHT_SUCCESS;
}
/*---------------------------------------------------------------------------*/