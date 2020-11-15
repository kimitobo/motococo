/*
LDR, 10/40 - 80/320 (room - flash)
MIC 43/174 - +/-10/20

*/

const int POTI_RIGHT = A0; 
uint16_t sensorValue = 0;
uint8_t mappedValue = 0;  

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(POTI_RIGHT);
  mappedValue = map(sensorValue, 0, 1023, 0, 255);
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  Serial.print("\t mapped = ");
  Serial.println(mappedValue);
  delay(2);
}
