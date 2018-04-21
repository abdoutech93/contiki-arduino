#ifndef __DHT11_H__
#define __DHT11_H__
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "lib/sensors.h"
/* DHT 1-wire is at PortD.4 */
#define DHT11_PIN_READ PIND7
#define DHT11_PIN_MASK _BV(PD7)
#define DHT11_PxOUT PORTD
#define DHT11_PxDIR DDRD


/*
 * Sensor's port
 */
#define DDR_DHT11 DDRD
#define DHT11_PORT_BASE PORTD
//#define PIN_DHT PIND2

#ifdef DHT11_CONF_PIN
#define DHT11_PIN        DHT11_CONF_PIN
#else
#define DHT11_PIN        DHT11_PIN_READ
#endif
#ifdef DHT11_CONF_PORT
#define DHT11_PORT       DHT11_CONF_PORT
#else
#define DHT11_PORT       (digitalPinToPort(DHT11_PIN))
#endif
/** @} */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
#define DHT11_SENSOR "DHT11 sensor"
/* -------------------------------------------------------------------------- */
extern const struct sensors_sensor dht11;

/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT22 available commands
 * @{
 */
#define DHT11_READ_TEMP         0x01
#define DHT11_READ_HUM          0x02
#define DHT11_READ_ALL          0x03
/** @} */
/* -------------------------------------------------------------------------- */
/**
 * \name DHT22 return types
 * @{
 */
#define DHT11_ERROR             (-1)
#define DHT11_SUCCESS           0x00
#define DHT11_BUSY              0xFF
/* -------------------------------------------------------------------------- */
/**
 * \name DHT22 constants
 * @{
 */
#define DHT11_BUFFER            5    /**< Buffer to store the samples         */
#define DHT11_COUNT             8    /**< Minimum ticks to detect a "1" bit   */
#define DHT11_MAX_TIMMING       85   /**< Maximum ticks in a single operation */
#define DHT11_READING_DELAY     1                                 /**< 1 us   */
#define DHT11_READY_TIME        20                                /**< 40 us  */
#define DHT11_START_TIME        (RTIMER_SECOND / 50)              /**< 20 ms  */
#define DHT11_AWAKE_TIME        (RTIMER_SECOND / 4)               /**< 250 ms */
/** @} */
/* -------------------------------------------------------------------------- */
#define DHT22_SENSOR "DHT11 sensor"
/* -------------------------------------------------------------------------- */
extern const struct sensors_sensor dht11;


#define SET_PIN_INPUT() (DDR_DHT11 &= ~DHT11_PIN_MASK)
#define SET_PIN_OUTPUT() (DDR_DHT11 |= DHT11_PIN_MASK)

#define OUTP_0() (DHT11_PxOUT &= ~DHT11_PIN_MASK)
#define OUTP_1() (DHT11_PxOUT |= DHT11_PIN_MASK) 

#define PIN_INIT() do{  \
                     SET_PIN_INPUT();    \
                     OUTP_0();           \
                   } while(0)


/* Drive the one wire interface hight */
#define DHT_DRIVE() do {                    \
                     SET_PIN_OUTPUT();     \
                     OUTP_1();             \
                   } while (0)

/* Release the one wire by turning on the internal pull-up. */
#define DHT_RELEASE() do {                  \
                       SET_PIN_INPUT();    \
                       OUTP_1();           \
                     } while (0)

/* Read one bit. */
#define DHT_INP()  (DHT11_PIN & DHT11_PIN_MASK)

//The packet size is 40bit but each bit consists of low and high state
//so 40 x 2 = 80 transitions. Also we have 2 transistions DHT response
//and 2 transitions which indicates End Of Frame. In total 84
#define MAXTIMINGS  84
//This is the main function which requests and reads the packet
uint8_t DHT_Read_Data(uint16_t *temperature, uint16_t *humidity);
static int value(int type);
static int configure(int type, int value);
#endif 