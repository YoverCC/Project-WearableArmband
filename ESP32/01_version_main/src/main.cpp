#include <Arduino.h>
#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino

BluetoothSerial ESP_BT; //Object for Bluetooth

// Declaración de los pines
const int emgPin_1 = 36; //GPIO number - Sensor 01
const int emgPin_2 = 39; //GPIO number - Sensor 02
const int LED_BLUET = 27; //GPIO number - Bluetooth


// Variables
int incoming_bt = 0;

// Sensor 01
int emgValue_1 = 0; // Captured signal
int output_1 = 0; //Filtered signal
int output_abs_1 = 0; //Positive filtered signal
int output_threshold_1 = 0; //Threshold signal
double output_ma_1 = 0; // Moving Average signal
int Threshold_1 = 0; //Threshold
int temp_output_1 = 0; //Temporal variable of the max value

// Sensor 02
int emgValue_2 = 0; // Captured signal
int output_2 = 0; //Filtered signal
int output_abs_2 = 0; //Positive filtered signal
int output_threshold_2 = 0; //Threshold signal
double output_ma_2 = 0; // Moving Average signal
int Threshold_2 = 0; //Threshold
int temp_output_2 = 0; //Temporal variable of the max value

// Otras variables
int count = 0; //Counter
static int tiempo_reposo = 8; //Time in seconds
bool state_threshold = false; //State of setting threshold

unsigned long timer = 0;
long loopTime = 1000; //Microseconds

// Valores para la media móvil
const unsigned short dim_ma = 500;

/* FILTROS */

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


/* FUNCIÓN DE SINCRONIZACIÓN */
void timeSync(unsigned long deltaT){
  unsigned long currTime = micros();
  long timeToDelay = deltaT - (currTime - timer);
  if(timeToDelay > loopTime){
    delay(timeToDelay/1000);
    delayMicroseconds(timeToDelay % 1000);
  }
  else if (timeToDelay>0){
    delayMicroseconds(timeToDelay);
  }
  else{
    // timeToDelay es negativo, por lo que se empieza inmediatamente
  }
  timer = currTime + timeToDelay;
}

/* SIMPLE MOVING AVERAGE */

class simple_ma {
  private:
    // Simple media móvil
    const static unsigned short default_ma_length = 500;
    // Lenght of the filter
    unsigned short filter_length = dim_ma;
    // Vector with raw data
    double *data;
    double sum, average;
    int index;
    bool ma_complete;

  public:
    
    void init(){
      ma_complete = false;
      index = -1;
      sum = 0;
      average = 0;
      data = new double[filter_length];
      clear();
    }
    
    void clear(){
      for(unsigned short i = 0; i<filter_length; i++){
        data[i] = 0;
      }
    }

    void add(double x){
      index = (index + 1)&filter_length;
      sum -= data[index];
      data [index] = x;
      sum +=x;
      if(!ma_complete && (index == filter_length-1)){
        ma_complete = true;
      }
      if(ma_complete){
        average = sum/filter_length;
      } else{
        average = sum/(index+1);
      }
    }

    double getCurrentAverage(){
      return average;
    }

    double* getData(){
      return data;
    }

    unsigned short getFilterLength(){
      return filter_length;
    }
};

/* PROGRAMA PRINCIPAL*/

EMG_FILTER my_emg_filter_1;
//simple_ma my_ma_filter_1;

EMG_FILTER my_emg_filter_2;
//simple_ma my_ma_filter_2;

void setup() {
  // Comunicacion serial
  Serial.begin(115200);

  // Comunicacion bluetooth
  ESP_BT.begin("Wearable_armband"); //Name of your Bluetooth Signal
  pinMode (LED_BLUET, OUTPUT);//Specify that LED pin is output

  // Declaracion del filtro
  // Filtro sensor 01
  my_emg_filter_1.init(true,true,true,true);
  //my_ma_filter_1.init();
  // Filtro sensor 02
  my_emg_filter_2.init(true,true,true,true);
  //my_ma_filter_2.init();
  delay(1000);
}

void loop() {
  timeSync(loopTime);
  //Reading the signal emg - Sensor 01
  emgValue_1 = analogRead(emgPin_1);
  output_1 = my_emg_filter_1.update(emgValue_1);
  output_abs_1 = abs(output_1);

  //Reading the signal emg - Sensor 02
  emgValue_2 = analogRead(emgPin_2);
  output_2 = my_emg_filter_2.update(emgValue_2);
  output_abs_2 = abs(output_2);
  

  if(state_threshold){
    // Por cada inicio de conexión las señales
    // en los 3 primeros segundos no son tomados en cuenta
    if(count > 3000){
      // Sensor 01
      if(output_abs_1 > temp_output_1){
        temp_output_1 = output_abs_1;
      }
      // Sensor 02
      if(output_abs_2 > temp_output_2){
        temp_output_2 = output_abs_2;
      }
    }  
    count ++;
    if(count > tiempo_reposo*1000){
      Threshold_1 = temp_output_1;
      Threshold_2 = temp_output_2;
      state_threshold = false;
    }
    output_threshold_1 = output_abs_1;
    output_threshold_2 = output_abs_2;
  }
  else{
    // Sensor 01
    if(output_abs_1<Threshold_1){
     output_threshold_1 = 0;
    }
    else
    {
      output_threshold_1 = output_abs_1;
    }
    // Sensor 02
    if(output_abs_2<Threshold_2){
     output_threshold_2 = 0;
    }
    else
    {
      output_threshold_2 = output_abs_2;
    }
  }
  //my_ma_filter.add(output_threshold);
  //output_ma = my_ma_filter.getCurrentAverage();
  Serial.print(output_threshold_1);
  Serial.print(';');
  Serial.println(output_threshold_2);

  if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {
    incoming_bt = ESP_BT.read(); //Read what we recevive   
  }
  if (incoming_bt == 49)
  {
    digitalWrite(LED_BLUET, HIGH);
    ESP_BT.print(output_threshold_1);
    ESP_BT.print(';');
    ESP_BT.println(output_threshold_2);
  }
        
  if (incoming_bt == 48)
  {
    digitalWrite(LED_BLUET, LOW);
  }   
}