/*
 * Copyright (c) 2015, Zolertia <http://www.zolertia.com>
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
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup motion-sensor
 * @{
 *
 * \file
 *         Digital motion sensor driver
 * \author
 *         Abdeldjallil SOAL <abdeldjallilsoal@gmail.com>
 */
/*---------------------------------------------------------------------------*/
#include "Arduino.h"
#include <stdio.h>
#include "contiki.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "dev/pir-sensor.h"
#include "lib/sensors.h"
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
#define MOTION_SENSOR_PORT_BASE  GPIO_PORT_TO_BASE(MOTION_SENSOR_PORT)
#define MOTION_SENSOR_PIN_MASK   GPIO_PIN_MASK(MOTION_SENSOR_PIN)
/*---------------------------------------------------------------------------*/
//extern const struct sensors_sensor pir_sensor;
void (*presence_int_callback)(uint8_t value);
static int enabled = 0;
static struct timer debouncetimer;
static int status(int type);
//struct sensors_sensor *sensors[1];
//unsigned char sensors_flags[1];

#define PIR_BIT PD4
#define PIR_CHECK_IRQ() (EIFR & PIR_BIT) ? 0 : 1
/*---------------------------------------------------------------------------*/
ISR(PCINT2_vect)
{
  //printf("*** ISR DETECTED ....!\n");
  //leds_toggle(LEDS_YELLOW);
  
  //if(PIR_CHECK_IRQ()) {
    if(timer_expired(&debouncetimer)) {
    //led1_on();
      sensors_changed(&pir_sensor);
      timer_set(&debouncetimer, CLOCK_SECOND*60);
      //printf("*** MOTION DETECTED ....!\n");
      
    //led1_off();
    //}
  }

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
  return MOTION_SUCCESS;
}
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  printf("*** Value ....!\n");
  return (PORTD & _BV(PD4) ? 0 : 1) || !timer_expired(&debouncetimer);
  return GPIO_READ_PIN(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
}
/*---------------------------------------------------------------------------
static int
configure(int type, int value)
{
  if(type != MOTION_ACTIVE) {
    PRINTF("Motion: invalid configuration option\n");
    return MOTION_ERROR;
  }

  if(!value) {
    presence_int_callback = NULL;
    GPIO_DISABLE_INTERRUPT(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
    return MOTION_SUCCESS;
  }

  // Configure interruption 
  GPIO_SOFTWARE_CONTROL(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
  GPIO_SET_INPUT(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
  GPIO_DETECT_RISING(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
  GPIO_TRIGGER_SINGLE_EDGE(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
  //ioc_set_over(MOTION_SENSOR_PORT, MOTION_SENSOR_PIN, IOC_OVERRIDE_DIS);
  gpio_register_callback(motion_interrupt_handler, MOTION_SENSOR_PORT,
                         MOTION_SENSOR_PIN);

  process_start(&motion_int_process, NULL);

  GPIO_ENABLE_INTERRUPT(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);
  nvic_interrupt_enable(MOTION_SENSOR_VECTOR);
  return MOTION_SUCCESS;
}
---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
	switch (type) {
	case SENSORS_ACTIVE:
		if (c) {
			if(!status(SENSORS_ACTIVE)) {
  //  led1_on();
				timer_set(&debouncetimer, 0);
				GPIO_SET_INPUT(MOTION_SENSOR_PORT_BASE, MOTION_SENSOR_PIN_MASK);//pinMode(MOTION_SENSOR_PIN,INPUT) // Set pin as input
				//PORTD |= (1<<PORTD4); // Set port PORTE bint 6 with pullup resistor
				PCICR |= (1 << PCIE2);
        //GPIO_DETECT_RISING(MOTION_SENSOR_PORT_BASE,MOTION_SENSOR_PIN_MASK);//EICRB |= (3<<ISC60); // For rising edge
        PCMSK2 |= (1 << PCINT20);
				//EIMSK |= (1<<PD4); // Set int (INT6==PB6)
				enabled = 1;
				sei();
  //  led1_off();
          printf("*** Config ....!\n");
			}
		} else {
				enabled = 0;
				EIMSK &= ~(1<<PD4); // clear int
        printf("*** Config ERROR!\n");
		}
    printf("*** Config OK !\n");
		return 1;
	}
  printf("*** Config OK\n");
	return MOTION_SUCCESS;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(pir_sensor, MOTION_SENSOR, value, configure, status);
SENSORS(&pir_sensor);
/*---------------------------------------------------------------------------*/
/** @} */