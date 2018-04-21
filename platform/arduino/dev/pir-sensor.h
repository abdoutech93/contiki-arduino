#ifndef PIR_SENSOR_H_
#define PIR_SENSOR_H_

#include "lib/sensors.h"

extern const struct sensors_sensor pir_sensor;

#define MOTION_SENSOR "PIR"
/**
 * \name Motion sensor return and operation values
 * @{
 */
#define MOTION_ACTIVE     SENSORS_ACTIVE
#define MOTION_SUCCESS    0
#define MOTION_ERROR      (-1)
#ifdef PIR_CONF_PIN
#define MOTION_SENSOR_PIN        PIR_CONF_PIN
#else
#define MOTION_SENSOR_PIN        PIND4
#endif
#ifdef PIR_CONF_PORT
#define MOTION_SENSOR_PORT       PIR_CONF_PORT
#else
#define MOTION_SENSOR_PORT       (digitalPinToPort(MOTION_SENSOR_PIN))
#endif
#ifdef PIR_CONF_VECTOR
#define MOTION_SENSOR_VECTOR       PIR_CONF_VECTOR
#else
#define MOTION_SENSOR_VECTOR       NOT_A_PORT
#endif

#endif /* PIR_SENSOR_H_ */