/*
 * Copyright: Toshikazu
 *
 * <Description>
 * This program is for testing the latency from a response
 * to the reinforcer it produces in a concurrent VI VI schedule.
 * 
 */

byte Response1 = 2;
byte Relay = 3;
byte Response2 = 4;

void setup()
{
  Serial.begin(9600);
  pinMode(Response1, INPUT_PULLUP);
  pinMode(Response2, INPUT_PULLUP);
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, LOW); 
}

void loop()
{
  if (Serial.available() > 0)
  {
    char Event = Serial.read();
    switch (Event)
    {
      case 'r':
        digitalWrite(Relay, HIGH);
        delay(20);
        digitalWrite(Relay, LOW);
        delay(20);
        break;
    }
  }
  Serial.println(String(digitalRead(Response1))+String(digitalRead(Response2)));
  delay(4);
}

