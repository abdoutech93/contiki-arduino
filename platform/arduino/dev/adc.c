#include <avr/io.h>
#include <stdint.h>       // needed for uint8_t

int ADCsingleREAD(uint8_t adctouse)
{
    int ADCval;

    ADMUX = adctouse;        // use #1 ADC
    ADMUX |= (1 << REFS0);   // use AVcc as the reference
    ADMUX &= ~(1 << ADLAR);  // clear for 10 bit resolution

    // 128 prescale for 8Mhz
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    ADCSRA |= (1 << ADEN);    // Enable the ADC
    ADCSRA |= (1 << ADSC);    // Start the ADC conversion

    while(ADCSRA & (1 << ADSC)); // waits for the ADC to finish

    ADCval = ADCL;
    ADCval = (ADCH << 8) + ADCval; // ADCH is read so ADC can be updated again

    return ADCval;
}