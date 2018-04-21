/*
 DHT-11 Library
 (c) Created by Charalampos Andrianakis on 18/12/11.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
#include "Arduino.h"
#include <avr/io.h>
#include "contiki.h"
#include "dht11.h"
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
#define DHT_COUNT 6
#define DHT_MAXTIMINGS 84
extern const struct sensors_sensor dht11;
SENSORS_SENSOR(dht11, DHT11_SENSOR, value, configure, NULL);
/*---------------------------------------------------------------------------*/
static uint8_t enabled;
static uint8_t dht11_data[DHT11_BUFFER];
static uint8_t busy;
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];
/*---------------------------------------------------------------------------*/
#define udelay(u) clock_delay_usec(u)
#define mdelay(u) clock_delay_msec(u)
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static int
dht11_read(void)
{
  uint8_t i;
  uint8_t j = 0;
  uint8_t last_state;
  uint8_t counter = 0;
  uint8_t checksum = 0;

  if(enabled) {
    PRINTF("DHT11 IS ENABLED\n");
    /* Exit low power mode and initialize variables */
    GPIO_SET_OUTPUT(DHT11_PORT_BASE, DHT11_PIN_MASK);
    PRINTF("A1\n");
    GPIO_SET_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);
    PRINTF("A2\n");
    mdelay(DHT11_AWAKE_TIME);  
    //BUSYWAIT_UNTIL(DHT22_AWAKE_TIME);
    PRINTF("A3\n");
    memset(dht11_data, 0, DHT11_BUFFER);
    PRINTF("Initialization sequence\n");
    /* Initialization sequence */
    GPIO_CLR_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);
    mdelay(DHT11_START_TIME);
    //BUSYWAIT_UNTIL(DHT11_START_TIME);
    GPIO_SET_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);
    clock_delay_usec(DHT11_READY_TIME);

    /* Prepare to read, DHT22 should keep line low 80us, then 80us high.
     * The ready-to-send-bit condition is the line kept low for 50us, then if
     * the line is high between 24-25us the bit sent will be "0" (zero), else
     * if the line is high between 70-74us the bit sent will be "1" (one).
     */
    GPIO_SET_INPUT(DHT11_PORT_BASE, DHT11_PIN_MASK);
    last_state = GPIO_READ_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);

    for(i = 0; i < DHT11_MAX_TIMMING; i++) {
      counter = 0;
      while(GPIO_READ_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK) == last_state) {
        counter++;
        clock_delay_usec(DHT11_READING_DELAY);

        /* Exit if not responsive */
        if(counter == 0xFF) {
          break;
        }
      }

      last_state = GPIO_READ_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);

      /* Double check for stray sensor */
      if(counter == 0xFF) {
        break;
      }

      /* Ignore the first 3 transitions (the 80us x 2 start condition plus the
       * first ready-to-send-bit state), and discard ready-to-send-bit counts
       */
      if((i >= 4) && ((i % 2) == 0)) {
        dht11_data[j / 8] <<= 1;
        if(counter > DHT11_COUNT) {
          dht11_data[j / 8] |= 1;
        }
        j++;
      }
    }

    for(i = 0; i < DHT11_BUFFER; i++) {
      PRINTF("DHT11: (%u) %u\n", i, dht11_data[i]);
    }

    /* If we have 5 bytes (40 bits), wrap-up and end */
    if(j >= 40) {
      /* The first 2 bytes are humidity values, the next 2 are temperature, the
       * final byte is the checksum
       */
      checksum = dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3];
      checksum &= 0xFF;
      if(dht11_data[4] == checksum) {
        GPIO_SET_INPUT(DHT11_PORT_BASE, DHT11_PIN_MASK);
        GPIO_SET_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);
        return DHT11_SUCCESS;
      }
      PRINTF("DHT11: bad checksum\n");
    }
  }
  return DHT11_ERROR;
}
/*---------------------------------------------------------------------------*/
static uint16_t
dht11_humidity(void)
{
  uint16_t res;
  res = dht11_data[0];
  res *= 256;
  res += dht11_data[1];
  busy = 0;
  return res;
}
/*---------------------------------------------------------------------------*/
static uint16_t
dht11_temperature(void)
{
  uint16_t res;
  res = dht11_data[2] & 0x7F;
  res *= 256;
  res += dht11_data[3];
  busy = 0;
  return res;
}
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  if((type != DHT11_READ_HUM) && (type != DHT11_READ_TEMP) &&
     (type != DHT11_READ_ALL)) {
    PRINTF("DHT11: Invalid type %u\n", type);
    return DHT11_ERROR;
  }

  if(busy) {
    PRINTF("DHT11: ongoing operation, wait\n");
    return DHT11_BUSY;
  }

  busy = 1;

  if(dht11_read() != DHT11_SUCCESS) {
    PRINTF("DHT11: Fail to read sensor\n");
    GPIO_SET_INPUT(DHT11_PORT_BASE, DHT11_PIN_MASK);
    GPIO_SET_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);
    busy = 0;
    return DHT11_ERROR;
  }

  switch(type) {
  case DHT11_READ_HUM:
    return dht11_humidity();
  case DHT11_READ_TEMP:
    return dht11_temperature();
  case DHT11_READ_ALL:
    return DHT11_SUCCESS;
  default:
    return DHT11_ERROR;
  }
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int value)
{
  if(type != SENSORS_ACTIVE) {
    printf("ERROR ACTIVATION\n");
    return DHT11_ERROR;
  }

  //GPIO_SOFTWARE_CONTROL(DHT11_PORT_BASE, DHT11_PIN_MASK);
  //GPIO_SET_INPUT(DHT11_PORT_BASE, DHT11_PIN_MASK);
  //ioc_set_over(DHT22_PORT, DHT22_PIN, IOC_OVERRIDE_OE);
  //GPIO_SET_PIN(DHT11_PORT_BASE, DHT11_PIN_MASK);
  pinMode(DHT11_PIN,INPUT_PULLUP);
  /* Restart flag */
  busy = 0;

  if(value) {
    printf("SENSOR ENABLED\n");
    enabled = 1;
    return DHT11_SUCCESS;
  }
  printf("SENSOR NOT ENABLED\n");
  enabled = 0;
  return DHT11_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// todo: set DHT22 or DHT11 in project file
// define for DHT11 else for DHT22, RHT03 
//   #define DHT11	1

uint8_t DHT_Read_Data(uint16_t *temperature, uint16_t *humidity){

    //data[5] is 8byte table where data come from DHT are stored
    //laststate holds laststate value
    //counter is used to count microSeconds
    uint8_t data[5], laststate = 0, counter = 0, j = 0, i = 0;
    
    //Clear array
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    uint8_t volatile sreg;
    sreg = SREG;    /* Save status register before disabling interrupts. */
    cli();          /* Disable interrupts. */

    //Set pin Output
    //Pin High
    DHT_DRIVE();
    mdelay(100);                     //Wait for 100mS
    
    //Send Request Signal
    //Pin Low
    OUTP_0();                      //20ms Low 
    mdelay(20);
    //Pin High
    OUTP_1();
    udelay(40);                      //40us High
    
    //Set pin Input to read Bus
    //Set pin Input
    DHT_RELEASE();
    laststate=DHT_INP();             //Read Pin value
    
    //Repeat for each Transistions
    for (i=0; i<MAXTIMINGS; i++) {
        //While state is the same count microseconds
   //led1_on();
   //led1_off();
 
       while (laststate==DHT_INP()) {
            udelay(1);
            counter++;
            if (counter>254) break;
        }

        if (counter>254) break;
        
        //laststate==_BV(DHT_PIN) checks if laststate was High
        //ignore the first 2 transitions which are the DHT Response
        //if (laststate==_BV(DHT_PIN) && (i > 2)) {
        if ((i&0x01) && (i > 2)) {
            //Save bits in segments of bytes
            //Shift data[] value 1 position left
            //Example. 01010100 if we shift it left one time it will be
            //10101000
 
            data[j/8]<<=1;
            if (counter >= 15) {    //If it was high for more than 40uS
   //led1_on();
                data[j/8]|=1;       //it means it is bit '1' so make a logic
   //led1_off();
            }                       //OR with the value (save it)
            j++;                    //making an OR by 1 to this value 10101000
        }                           //we will have the resault 10101001
                                    //1 in 8-bit binary is 00000001
        //j/8 changes table record every 8 bits which means a byte has been saved
        //so change to next record. 0/8=0 1/8=0 ... 7/8=0 8/8=1 ... 15/8=1 16/8=2
        laststate=DHT_INP();     //save current state
        counter=0;                  //reset counter
        
    }
    SREG = sreg;    /* Enable interrupts. */
    printf("HUM %d %d %d %d %d %d \n",data[0],data[1],data[2],data[3],data[4],(uint8_t)(data[0] + data[1] + data[2] + data[3]) );
    //Check if data received are correct by checking the CheckSum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4]) {
#ifdef DHT11
        *humidity = data[0]*100;
        *temperature = data[2]*100;
#else
        *humidity = ((uint16_t)data[0]<<8 | data[1])*10;
        *temperature = ((uint16_t)data[2]<<8 | data[3])*10;
#endif
        return 0;
    }else{
        *humidity = 2;
        *temperature = 2;
//        uart_puts("\r\nCheck Sum Error");
    }
 
     return 0xff;  // Check Sum Error
}