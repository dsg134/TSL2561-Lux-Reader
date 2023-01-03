// This program is a test for the TSL 2561 light sensor module. A certain amount of lux samples are gathered, which are then used to determine the mean and signal variance of the output. 

#include <Wire.h>
#include "TSL2561.h"

TSL2561 tsl(TSL2561_ADDR_FLOAT); 

uint16_t counter = 1;
uint16_t number_of_samples = 100;
uint16_t sample_lux_store[100]; // 100 = # samples taken from lux signal to calculate mean and variance

double average(uint16_t array[], uint16_t num_samples) { // calculate the mean of a sampled signal
    double sum = 0;
    for (int i = 0; i < num_samples; i++)
       sum += array[i];
 
    return (double)(sum / num_samples);
}

double signal_variance(uint16_t array[], uint16_t num_samples, double mean) { // calculate the variance of a sampled signal
    double variance = 0;
    for (int i = 0; i < num_samples; i++)
       array[i] = array[i] - mean;
    for (int j = 0; j < num_samples; j++)
       variance += (array[j] * array[j]);
       
    return (double) (variance / num_samples); 
}

void setup(void) {
  Serial.begin(9600);
  
  if (tsl.begin()) {
    Serial.println("Sensor Found");
  } else {
    Serial.println("Sensor Not Detected");
    while (1);
  }
    
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
}

void loop(void) {
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE); // lux calculation is based off of visible light     
  //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM); // full spectrum option
  //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED); // IR option
  
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;

  Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));
  delay(100); 
  
  if (counter <= number_of_samples) {
    sample_lux_store[(counter - 1)] = tsl.calculateLux(full, ir); // stores the first 100 lux samples for mean and variance calculations

    double mean_of_lux_values = average(sample_lux_store, counter); // calculates the average of the signal
    Serial.print("Mean: "); Serial.println(mean_of_lux_values); // check to see how the mean is updating for 1st 100 samples being collected

    if (counter == number_of_samples) {
        double variance_of_lux_signal = signal_variance(sample_lux_store, number_of_samples, mean_of_lux_values); // signal variance calculation
        Serial.print("Signal Variance: "); Serial.println(variance_of_lux_signal);
    }
    
    counter++; 
  }
}
