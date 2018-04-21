/* struct dht22 AVR Lirary
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

#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdint.h>
#include "lib/sensors.h"
/*
 * Sensor's port
 */
#define DDR_DHT DDRB
#define PORT_DHT PORTB
#define PIN_DHT PINB
#ifdef DHT22_CONF_PIN
#define DHT22_PIN        DHT22_CONF_PIN
#else
#define DHT22_PIN        NOT_A_PIN
#endif
#ifdef DHT22_CONF_PORT
#define DHT22_PORT       DHT22_CONF_PORT
#else
#define DHT22_PORT       (digitalPinToPort(DHT22_PIN))
#endif
/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT22 available commands
 * @{
 */
#define DHT22_READ_TEMP         0x01
#define DHT22_READ_HUM          0x02
#define DHT22_READ_ALL          0x03
/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT22 return types
 * @{
 */
#define DHT22_ERROR             (-1)
#define DHT22_SUCCESS           0x00
#define DHT22_BUSY              0xFF
/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT22 constants
 * @{
 */
#define DHT22_BUFFER            5    /**< Buffer to store the samples         */
#define DHT22_COUNT             8    /**< Minimum ticks to detect a "1" bit   */
#define DHT22_MAX_TIMMING       85   /**< Maximum ticks in a single operation */
#define DHT22_READING_DELAY     1                                 /**< 1 us   */
#define DHT22_READY_TIME        20                                /**< 40 us  */
#define DHT22_START_TIME        (RTIMER_SECOND / 50)              /**< 20 ms  */
#define DHT22_AWAKE_TIME        (RTIMER_SECOND / 4)               /**< 250 ms */
/** @} */
/* -------------------------------------------------------------------------- */
#define DHT22_SENSOR "DHT22 sensor"
/* -------------------------------------------------------------------------- */
extern const struct sensors_sensor dht22;
/**
 * Init dht sensor
 * @dht: sensor struct
 * @pin: PORT & DDR pin
 */
void dht_init(struct dht22 *dht, uint8_t pin);

/**
 * Reading temperature from sensor
 * @dht: sensor struct
 * @temp: out temperature pointer
 *
 * Returns 1 if succeful reading
 * Returns 0 if fail reading
 */
uint8_t dht_read_temp(struct dht22 *dht, float *temp);

/**
 * Reading humidity from sensor
 * @dht: sensor struct
 * @hum: out humidity pointer
 *
 * Returns 1 if succeful reading
 * Returns 0 if fail reading
 */
uint8_t dht_read_hum(struct dht22 *dht, float *hum);

/**
 * Reading temperature and humidity from sensor
 * @dht: sensor struct
 * @temp: out temperature pointer
 * @hum: out humidity pointer
 *
 * Returns 1 if succeful reading
 * Returns 0 if fail reading
 *
 * The fastest function for getting temperature + humidity.
 */
uint8_t dht_read_data(struct dht22 *dht, float *temp, float *hum);
int dht22_read_all(int *temperature, int *humidity);
static int value(int type);
static int configure(int type, int value);
#endif