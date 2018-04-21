#include "contiki.h"
#include "lib/sensors.h"
#include "dev/dht.h"
#include "dev/leds.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(remote_dht11_process, "DHT11 test");
AUTOSTART_PROCESSES(&remote_dht11_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(remote_dht11_process, ev, data)
{
  int16_t temperature, humidity;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(dht);

  /* Let it spin and read sensor data */

  while(1) {
    etimer_set(&et, CLOCK_SECOND*30);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /* The standard sensor API may be used to read sensors individually, using
     * the `dht22.value(DHT22_READ_TEMP)` and `dht22.value(DHT22_READ_HUM)`,
     * however a single read operation (5ms) returns both values, so by using
     * the function below we save one extra operation
     */
    if(DHT_Read_Data(&temperature, &humidity) == DHT_SUCCESS) {
      printf("Temperature %02d.%02d ºC, ", temperature, 0);
      printf("Humidity %02d.%02d RH\n", humidity,0);
    } else {
      printf("Failed to read the sensor\n");
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
