unsigned long timer = 0;
long loopTime = 5000; //microseconds
int sensorPin01 = A1;
int sensorPin02 = A2;

void setup() {
  Serial.begin(460800);
  timer = micros();
}

void loop() {
  timeSync(loopTime);
  analogReadResolution(10);
  float val01 = analogRead(sensorPin01);
  float val02 = analogRead(sensorPin02);
  Serial.println(val01);
  //sendToPC(&val01,&val02);
}

void timeSync(unsigned long deltaT){
  unsigned long currTime = micros();
  long timeToDelay = deltaT - (currTime - timer);
  if(timeToDelay > 5000)
  {
    delay(timeToDelay / 1000);
    delayMicroseconds(timeToDelay % 1000);
  }
  else if (timeToDelay > 0)
  {
    delayMicroseconds(timeToDelay);
  }
  else
  {
    // timeToDelay es negativo, por lo que se empieza inmediatamente
  }
  timer = currTime + timeToDelay;
}

void sendToPC(float* data1, float* data2)
{
  byte* byteData1 = (byte*)(data1);
  byte* byteData2 = (byte*)(data2);
  byte buf[8] = {byteData1[0], byteData1[1],byteData1[2],byteData1[3],byteData2[0],byteData2[1],byteData2[2],byteData2[3]};
  Serial.write(buf, 8);
}
