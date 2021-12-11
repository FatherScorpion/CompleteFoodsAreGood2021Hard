void setup() {
  pinMode(12,INPUT);
  Serial.begin(9600);
}

void loop() {
  int value = digitalRead(12);
  Serial.println(value);
  delay(100);
}
