/* Este codigo esta basado en el codigo otorgado por el fabricante
 * Este codigo tiene por finalidad el autothreshold del sensor, para esto
 * se dejara un tiempo en el cual detectará el máximo valor para luego
 * asignar este al threshold, lo otro es generar el plot con dimensiones
 * fijas y además, considerando la amplitud en voltaje de la señal.
 * NOTA: Con bateria, la calibración sale distinta, los valores son más pequeños
 * Cuando la laptop esta conectado al tomacorriente se generan valores más altos.
 */
 
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EMGFilters.h"

#define TIMING_DEBUG 1

#define SensorInputPin A1 // input pin number

EMGFilters myFilter;
// discrete filters must works with fixed sample frequence
// our emg filter only support "SAMPLE_FREQ_500HZ" or "SAMPLE_FREQ_1000HZ"
// other sampleRate inputs will bypass all the EMG_FILTER
int sampleRate1 = SAMPLE_FREQ_1000HZ;
// For countries where power transmission is at 50 Hz
// For countries where power transmission is at 60 Hz, need to change to
// "NOTCH_FREQ_60HZ"
// our emg filter only support 50Hz and 60Hz input
// other inputs will bypass all the EMG_FILTER
int humFreq1 = NOTCH_FREQ_60HZ;

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the threshold;
// any value under threshold will be set to zero

int Threshold_inicial = 0; // Este valor debe ser autoseteado por el codigo
int Threshold_after = 0;
int maxval = 0;

int timeIter = 0;
unsigned long timeStamp;
unsigned long timeBudget;

void setup() {
    /* add setup code here */
    myFilter.init(sampleRate1, humFreq1, true, true, true);

    // open serial
    Serial.begin(9600);

    // setup for time cost measure
    // using micros()
    timeBudget = 1e6 / sampleRate1;
    // micros will overflow and auto return to zero every 70 minutes
}

void loop() {
    /* add main program code here */
    // In order to make sure the ADC sample frequence on arduino,
    // the time cost should be measured each loop
    /*------------start here-------------------*/
    int Value = analogRead(SensorInputPin);
    // filter processing
    int DataAfterFilter = myFilter.update(Value);
    int envlope = sq(DataAfterFilter);
    
    timeIter = timeIter + 1;
    if(timeIter > 10000){  
      // any value under threshold will be set to zero
      if(timeIter == 10001){
        Threshold_after = maxval-5000;
        }
      envlope = (envlope > Threshold_after) ? envlope : 0;
  
      if (TIMING_DEBUG) {
        
          Serial.print("Squared Data: ");
          Serial.println(envlope);
  
      }
    }
    else{
      envlope = (envlope > Threshold_inicial) ? envlope : 0;
      if(envlope>maxval){
        maxval = envlope;
        }
      if (TIMING_DEBUG) {
        Serial.print("Squared Data: ");
        Serial.println(envlope);
        }
        }
    /*------------end here---------------------*/

    delayMicroseconds(500);

}
