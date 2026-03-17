// put your setup code here, to run once:
void setup() {
  // LEDs
  pinMode(3, OUTPUT); // BREMSLICHT
  pinMode(5, OUTPUT); // FRONTLICHT
  pinMode(6, OUTPUT); // HUPE
  pinMode(7, OUTPUT); // BLINKER LINKS
  pinMode(8, OUTPUT); // BLINKER RECHTS
}

// put your main code here, to run repeatedly:
void loop() {
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(5, LOW);
  delay(1000);
}
