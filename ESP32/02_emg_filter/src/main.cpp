#include <Arduino.h>

const int emgPin = 36; //GPIO number
int emgValue = 0; // Captured signal
int output = 0; //Filtered signal
int output_abs = 0; //Positive filtered signal
int output_threshold = 0; //Threshold signal
int Threshold = 0; //Threshold
int temp_output = 0; //Temporal variable of the max value
int count = 0; //Counter
static int tiempo_reposo = 7; //Time in seconds
bool state_threshold = true; //State of setting threshold

// IIR notch filter 60Hz
static float ahf_60_numerator[1][3] = {{0.9876, -1.8365, 0.9876}};
static float ahf_60_denominator[1][3] = {{1 ,-1.8365, 0.9752}};

// IIR notch filter 120Hz
static float ahf_120_numerator[1][3] = {{0.9755, -1.4222, 0.9755}};
static float ahf_120_denominator[1][3] = {{1 ,-1.4222, 0.9510}};

// HighPass filter 20Hz
static float hp_numerator[1][3] = {{0.9150, -1.8299, 0.9150}};
static float hp_denominator[1][3] = {{1 ,-1.8227, 0.8372}};

// LowPass filter 500Hz
static float lp_numerator[1][3] = {{0.9978, 1.9956, 0.9978}};
static float lp_denominator[1][3] = {{1 ,1.9956, 0.9956}};

enum FILTER_TYPE {
    FILTER_TYPE_LOWPASS,
    FILTER_TYPE_HIGHPASS,
    FILTER_TYPE_NOTCH_60,
    FILTER_TYPE_NOTCH_120,
};

class FILTER_2nd {
  private:
    // second-order filter
    float states[2];
    float num[3];
    float den[3];

  public:
    void init(FILTER_TYPE ftype){
      states[0]=0;
      states[1]=0;

      if(ftype == FILTER_TYPE_LOWPASS){
        for (int i =0; i<3; i++){
        num[i] = lp_numerator[0][i];
        den[i] = lp_denominator[0][i];
        }
      } 

      if(ftype == FILTER_TYPE_HIGHPASS){
        for (int i =0; i<3; i++){
        num[i] = hp_numerator[0][i];
        den[i] = hp_denominator[0][i];
        }
      } 

      if(ftype == FILTER_TYPE_NOTCH_60){
        for (int i =0; i<3; i++){
        num[i] = ahf_60_numerator[0][i];
        den[i] = ahf_60_denominator[0][i];
        }
      }
      
      if(ftype == FILTER_TYPE_NOTCH_120){
        for (int i =0; i<3; i++){
        num[i] = ahf_120_numerator[0][i];
        den[i] = ahf_120_denominator[0][i];
        }
      } 
    }


    float update(float input){
      float tmp = (input-den[1]*states[0]-den[2]*states[1])/den[0];
      float output = num[0]*tmp + num[1]*states[0]+num[2]*states[1];
      // save last states
      states[1] = states[0];
      states[0] = tmp;
      return output;
    }
};
FILTER_2nd AHF_60;
FILTER_2nd AHF_120;
FILTER_2nd LPF;
FILTER_2nd HPF;

class EMG_FILTER{
  private:
    bool notchFilter60Enabled;
    bool notchFilter120Enabled;
    bool lowpassFilterEnabled;
    bool highpassFilterEnabled;
  
  public:

    void init(bool enableNotch60Filter = true,
              bool enableNotch120Filter = true,
              bool enableHighpassFilter = true,
              bool enableLowpassFilter = true){
      notchFilter60Enabled = enableNotch60Filter;
      notchFilter120Enabled = enableNotch120Filter;
      highpassFilterEnabled = enableHighpassFilter;
      lowpassFilterEnabled = enableLowpassFilter;
      AHF_60.init(FILTER_TYPE_NOTCH_60);
      AHF_120.init(FILTER_TYPE_NOTCH_120);
      LPF.init(FILTER_TYPE_LOWPASS);
      HPF.init(FILTER_TYPE_HIGHPASS);
    }

    int update(int inputValue){
      int output = 0;
      if(notchFilter60Enabled){
        output = AHF_60.update(inputValue);
      }
      if(notchFilter120Enabled){
        output = AHF_120.update(output);
      }
      if (highpassFilterEnabled){
        output = HPF.update(output);
      }    
      if(lowpassFilterEnabled){
        output = LPF.update(output);
      }

      return output;
    }

};


EMG_FILTER my_emg_filter;

void setup() {
  my_emg_filter.init(true,true,true,false);
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  //Reading the signal emg
  emgValue = analogRead(emgPin);
  output = my_emg_filter.update(emgValue);
  output_abs = abs(output);
  
  if(state_threshold){

    if(count > 3000){
      if(output_abs > temp_output){
        temp_output = output_abs;
      }
    }  
    count ++;
    if(count > tiempo_reposo*1000){
      Threshold = temp_output;
      state_threshold = false;
    }
    output_threshold = output_abs;
  }
  else
  {
    if(output_abs<Threshold){
     output_threshold = 0;
    }
    else
    {
      output_threshold = output_abs;
    }
  }
  Serial.println(output_threshold);
  delay(1);
}