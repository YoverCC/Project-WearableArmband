#include <Arduino.h>

const int emgPin = 36; //GPIO number
int emgValue = 0;
int output = 0;

static float ahf_numerator[1][3] = {{0.9946, -1.8496, 0.9946}};
static float ahf_denominator[1][3] = {{1 ,-1.8496, 0.9893}};

class FILTER_2nd {
  private:
    // second-order filter
    float states[2];
    float num[3];
    float den[3];

  public:
    void init(){
      states[0]=0;
      states[1]=0;
      for (int i =0; i<3; i++){
        num[i] = ahf_numerator[0][i];
        den[i] = ahf_denominator[0][i];
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

FILTER_2nd AHF;

void setup() {
  // put your setup code here, to run once:
  AHF.init();
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  emgValue = analogRead(emgPin);
  output = AHF.update(emgValue);
  Serial.println(output);
  delay(1);
}