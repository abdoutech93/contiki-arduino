/*
 * Copyright (c) 2015, Copyright Robert Olsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
#include "Arduino.h"
#include "contiki.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "lib/sensors.h"
#include "dev/light-sensor.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
const struct sensors_sensor light_sensor;
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];
static int enabled = 0;
static int busy = 0;
#define LDR_SENSOR_PORT_BASE  GPIO_PORT_TO_BASE(LDR_SENSOR_PORT)
#define LDR_SENSOR_PIN_MASK   GPIO_PIN_MASK(LDR_SENSOR_PIN)
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
   return GPIO_READ_PIN(LDR_SENSOR_PORT_BASE, LDR_SENSOR_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
uint8_t LDR_Read_Data(uint16_t *value){
    *value = digitalRead(LDR_SENSOR_PIN);
    
    printf("RONDOM VALUE : %d\n",GPIO_READ_PIN(LDR_SENSOR_PORT_BASE, LDR_SENSOR_PIN_MASK)); 
    printf("LDR BRUTE : %d\n",digitalRead(LDR_SENSOR_PIN));
    printf("LDR VALUE : %d\n",value);
    return LDR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
   switch (type) {
	case SENSORS_ACTIVE:
	case SENSORS_READY:
		return enabled;//(EIMSK & (1<<INT6) ? 0 : 1);//PIR_IRQ_ENABLED();
	}
  return LDR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
   if(type != SENSORS_ACTIVE) {
    return LDR_ERROR;
  }

  //GPIO_SOFTWARE_CONTROL(LDR_SENSOR_PORT_BASE, LDR_SENSOR_PIN_MASK);
  //GPIO_SET_INPUT(LDR_SENSOR_PORT_BASE, LDR_SENSOR_PIN_MASK);
  //ioc_set_over(LDR_PORT, LDR_PIN, IOC_OVERRIDE_OE);
  //GPIO_SET_PIN(LDR_SENSOR_PORT_BASE, LDR_SENSOR_PIN_MASK);
  pinMode(LDR_SENSOR_PIN,INPUT);
  /* Restart flag */
  busy = 0;

  if(value) {
    enabled = 1;
    return LDR_SUCCESS;
  }

  enabled = 0;
  return LDR_ERROR;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(light_sensor, LDR_SENSOR, value, configure, status);