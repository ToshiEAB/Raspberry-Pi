/*
 * Copyright: Toshikazu
 *
 * <Description>
 * This program is for testing the latency from a response
 * to the reinforcer it produces in an FR schedule.
 * 
 */

byte Response = 2;
byte Relay = 3;

void setup()
{
  Serial.begin(9600);
  pinMode(Response, INPUT_PULLUP);
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
  Serial.println(digitalRead(Response));
  delay(4);
}

