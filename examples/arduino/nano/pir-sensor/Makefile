CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = test-motion
CONTIKI_SOURCEFILES += pir-sensor.c sensors.c
TARGET = arduino
ARDUINO_MODEL = Uno
all: $(CONTIKI_PROJECT)

#UIP_CONF_IPV6=1

CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include
