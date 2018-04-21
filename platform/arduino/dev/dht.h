#ifndef __DHT_H__
#define __DHT_H__
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "lib/sensors.h"
/* -------------------------------------------------------------------------- */
#ifdef DHT_CONF_TYPE
#define DHT_TYPE        DHT_CONF_TYPE
#else
#define DHT_TYPE        DHT11
#endif
#ifdef DHT_CONF_PIN
#define DHT_PIN        DHT_CONF_PIN
#else
#define DHT_PIN        PIND7
#endif
#ifdef DHT_CONF_PORT
#define DHT_PORT       DHT_CONF_PORT
#else
#define DHT_PORT       (digitalPinToPort(DHT_PIN))
#endif
/** @} */
/* -------------------------------------------------------------------------- */
#define DHT_SENSOR "DHT sensor"
/* -------------------------------------------------------------------------- */
extern const struct sensors_sensor dht;
/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT available commands
 * @{
 */
#define DHT_READ_TEMP         0x01
#define DHT_READ_HUM          0x02
#define DHT_READ_ALL          0x03
/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT return types
 * @{
 */
#define DHT_ERROR             (-1)
#define DHT_SUCCESS           0x00
#define DHT_BUSY              0xFF
/* -------------------------------------------------------------------------- */
// Define types of sensors.
#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21
/* -------------------------------------------------------------------------- */
#define MIN_INTERVAL 2000
#define MAXCYCLES 80
/* -------------------------------------------------------------------------- */
uint8_t data[5];
uint8_t _bit, _port;
uint32_t _lastreadtime;
uint32_t _maxcycles;
int _lastresult;
/* -------------------------------------------------------------------------- */
static int value(int type);
static int configure(int type, int value);
uint8_t DHT_Read_Data(uint16_t *temperature, uint16_t *humidity);
#endif 