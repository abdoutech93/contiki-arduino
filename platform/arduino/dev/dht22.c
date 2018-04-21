/* Fast DHT Lirary
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by Adafruit Industries. MIT license.
 */

#include "Arduino.h"
#include "dht22.h"
#include "gpio.h"
#include <avr/io.h>
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
#define DHT_COUNT 6
#define DHT_MAXTIMINGS 84

#define udelay(u) clock_delay_usec(u)
#define mdelay(u) clock_delay_msec(u)

/*void dht_init(struct dht22 *dht, uint8_t pin)
{
    dht->pin = pin;
    // Setup the pins! 
    DDR_DHT &= ~(1 << dht->pin);
    PORT_DHT |= (1 << dht->pin);
}*/

/*static uint8_t dht_read(struct dht22 *dht)
{
    uint8_t tmp;
    uint8_t sum = 0;
    uint8_t j = 0, i;
    uint8_t last_state = 1;
    uint16_t counter = 0;
    
     // Pull the pin 1 and wait 250 milliseconds
     
    PORT_DHT |= (1 << dht->pin);
    _delay_ms(250);

    dht->data[0] = dht->data[1] = dht->data[2] = dht->data[3] = dht->data[4] = 0;

    // Now pull it low for ~20 milliseconds 
    DDR_DHT |= (1 << dht->pin);
    PORT_DHT &= ~(1 << dht->pin);
    _delay_ms(20);
    cli();
    PORT_DHT |= (1 << dht->pin);
    _delay_us(40);
    DDR_DHT &= ~(1 << dht->pin);

    // Read the timings 
    for (i = 0; i < DHT_MAXTIMINGS; i++) {
        counter = 0;
        while (1) {
            tmp = ((PIN_DHT & (1 << dht->pin)) >> 1);
            _delay_us(3);

            if (tmp != last_state)
                break;

            counter++;
            _delay_us(1);

            if (counter == 255)
                break;
        }

        last_state = ((PIN_DHT & (1 << dht->pin)) >> 1);

        if (counter == 255)
            break;

        // Ignore first 3 transitions 
        if ((i >= 4) && (i % 2 == 0)) {
            // Shove each bit into the storage bytes 
            dht->data[j/8] <<= 1;
            if (counter > DHT_COUNT)
                dht->data[j/8] |= 1;
            j++;
        }
    }

    sei();
    sum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];

    if ((j >= 40) && (dht->data[4] == (sum & 0xFF)))
        return 1;
    return 0;
}*/

/*uint8_t dht_read_temp(struct dht22 *dht, float *temp)
{
    if (dht_read(dht)) {
        *temp = dht->data[2] & 0x7F;
        *temp *= 256;
        *temp += dht->data[3];
        *temp /= 10;

        if (dht->data[2] & 0x80)
            *temp *= -1;
        return 1;
    }
    return 0;
}*/

/*uint8_t dht_read_hum(struct dht22 *dht, float *hum)
{
    if (dht_read(dht)) {
        *hum = dht->data[0];
        *hum *= 256;
        *hum += dht->data[1];
        *hum /= 10;
        if (*hum == 0.0f)
            return 0;
        return 1;
    }
    return 0;
}*/

/*uint8_t dht_read_data(struct dht22 *dht, float *temp, float *hum)
{
    if (dht_read(dht)) {
        // Reading temperature 
        *temp = dht->data[2] & 0x7F;
        *temp *= 256;
        *temp += dht->data[3];
        *temp /= 10;

        if (dht->data[2] & 0x80)
            *temp *= -1;

        // Reading humidity 
        *hum = dht->data[0];
        *hum *= 256;
        *hum += dht->data[1];
        *hum /= 10;
        if (*hum == 0.0f)
            return 0;
        return 1;
    }
    return 0;
}*/
#define BUSYWAIT_UNTIL(max_time) \
  do { \
    rtimer_clock_t t0; \
    t0 = RTIMER_NOW(); \
    while(RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (max_time))) { \
      watchdog_periodic(); \
    } \
  } while(0)
/*---------------------------------------------------------------------------*/
#define DHT22_PORT_BASE          GPIO_PORT_TO_BASE(DHT22_PORT)
#define DHT22_PIN_MASK           GPIO_PIN_MASK(DHT22_PIN)
/*---------------------------------------------------------------------------*/
extern const struct sensors_sensor dht22;
SENSORS_SENSOR(dht22, DHT22_SENSOR, value, configure, NULL);
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static uint8_t enabled;
static uint8_t busy;
static uint8_t dht22_data[DHT22_BUFFER];
/*---------------------------------------------------------------------------*/
static int
dht22_read(void)
{
  uint8_t i;
  uint8_t j = 0;
  uint8_t last_state;
  uint8_t counter = 0;
  uint8_t checksum = 0;

  if(enabled) {
    PRINTF("DHT22 IS ENABLED\n");
    /* Exit low power mode and initialize variables */
    GPIO_SET_OUTPUT(DHT22_PORT_BASE, DHT22_PIN_MASK);
    PRINTF("A1\n");
    GPIO_SET_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);
    PRINTF("A2\n");
    mdelay(DHT22_AWAKE_TIME);  
    //BUSYWAIT_UNTIL(DHT22_AWAKE_TIME);
    PRINTF("A3\n");
    memset(dht22_data, 0, DHT22_BUFFER);
    PRINTF("Initialization sequence\n");
    /* Initialization sequence */
    GPIO_CLR_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);
    mdelay(DHT22_START_TIME);
    //BUSYWAIT_UNTIL(DHT22_START_TIME);
    GPIO_SET_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);
    clock_delay_usec(DHT22_READY_TIME);

    /* Prepare to read, DHT22 should keep line low 80us, then 80us high.
     * The ready-to-send-bit condition is the line kept low for 50us, then if
     * the line is high between 24-25us the bit sent will be "0" (zero), else
     * if the line is high between 70-74us the bit sent will be "1" (one).
     */
    GPIO_SET_INPUT(DHT22_PORT_BASE, DHT22_PIN_MASK);
    last_state = GPIO_READ_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);

    for(i = 0; i < DHT22_MAX_TIMMING; i++) {
      counter = 0;
      while(GPIO_READ_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK) == last_state) {
        counter++;
        clock_delay_usec(DHT22_READING_DELAY);

        /* Exit if not responsive */
        if(counter == 0xFF) {
          break;
        }
      }

      last_state = GPIO_READ_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);

      /* Double check for stray sensor */
      if(counter == 0xFF) {
        break;
      }

      /* Ignore the first 3 transitions (the 80us x 2 start condition plus the
       * first ready-to-send-bit state), and discard ready-to-send-bit counts
       */
      if((i >= 4) && ((i % 2) == 0)) {
        dht22_data[j / 8] <<= 1;
        if(counter > DHT22_COUNT) {
          dht22_data[j / 8] |= 1;
        }
        j++;
      }
    }

    for(i = 0; i < DHT22_BUFFER; i++) {
      PRINTF("DHT22: (%u) %u\n", i, dht22_data[i]);
    }

    /* If we have 5 bytes (40 bits), wrap-up and end */
    if(j >= 40) {
      /* The first 2 bytes are humidity values, the next 2 are temperature, the
       * final byte is the checksum
       */
      checksum = dht22_data[0] + dht22_data[1] + dht22_data[2] + dht22_data[3];
      checksum &= 0xFF;
      if(dht22_data[4] == checksum) {
        GPIO_SET_INPUT(DHT22_PORT_BASE, DHT22_PIN_MASK);
        GPIO_SET_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);
        return DHT22_SUCCESS;
      }
      PRINTF("DHT22: bad checksum\n");
    }
  }
  return DHT22_ERROR;
}
/*---------------------------------------------------------------------------*/
static uint16_t
dht22_humidity(void)
{
  uint16_t res;
  res = dht22_data[0];
  res *= 256;
  res += dht22_data[1];
  busy = 0;
  return res;
}
/*---------------------------------------------------------------------------*/
static uint16_t
dht22_temperature(void)
{
  uint16_t res;
  res = dht22_data[2] & 0x7F;
  res *= 256;
  res += dht22_data[3];
  busy = 0;
  return res;
}
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  if((type != DHT22_READ_HUM) && (type != DHT22_READ_TEMP) &&
     (type != DHT22_READ_ALL)) {
    PRINTF("DHT22: Invalid type %u\n", type);
    return DHT22_ERROR;
  }

  if(busy) {
    PRINTF("DHT22: ongoing operation, wait\n");
    return DHT22_BUSY;
  }

  busy = 1;

  if(dht22_read() != DHT22_SUCCESS) {
    PRINTF("DHT22: Fail to read sensor\n");
    GPIO_SET_INPUT(DHT22_PORT_BASE, DHT22_PIN_MASK);
    GPIO_SET_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);
    busy = 0;
    return DHT22_ERROR;
  }

  switch(type) {
  case DHT22_READ_HUM:
    return dht22_humidity();
  case DHT22_READ_TEMP:
    return dht22_temperature();
  case DHT22_READ_ALL:
    return DHT22_SUCCESS;
  default:
    return DHT22_ERROR;
  }
}
/*---------------------------------------------------------------------------*/
int
dht22_read_all(int *temperature, int *humidity)
{
  if((temperature == NULL) || (humidity == NULL)) {
    PRINTF("DHT22: Invalid arguments\n");
    return DHT22_ERROR;
  }

  if(value(DHT22_READ_ALL) != DHT22_ERROR) {
    *temperature = dht22_temperature();
    *humidity = dht22_humidity();
    return DHT22_SUCCESS;
  }

  /* Already cleaned-up in the value() function */
  return DHT22_ERROR;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int value)
{
  if(type != SENSORS_ACTIVE) {
    return DHT22_ERROR;
  }

  GPIO_SOFTWARE_CONTROL(DHT22_PORT_BASE, DHT22_PIN_MASK);
  GPIO_SET_INPUT(DHT22_PORT_BASE, DHT22_PIN_MASK);
  //ioc_set_over(DHT22_PORT, DHT22_PIN, IOC_OVERRIDE_OE);
  GPIO_SET_PIN(DHT22_PORT_BASE, DHT22_PIN_MASK);

  /* Restart flag */
  busy = 0;

  if(value) {
    enabled = 1;
    return DHT22_SUCCESS;
  }

  enabled = 0;
  return DHT22_SUCCESS;
}
