int sensorPin = A1;
int val_bit = 0;
int val_voltaje = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogReadResolution(10);
  val_bit = analogRead(sensorPin);
  //Serial.print("El valor de lectura en bits es: ");
  //Serial.println(val_bit);
  val_voltaje = val_bit*3300/1023;
  //Serial.print("El valor de lectura en voltaje es: ");
  Serial.println(val_voltaje);
  delay(10);
  }
