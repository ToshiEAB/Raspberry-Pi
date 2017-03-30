/*
 * Copyright: Toshikazu
 *
 * <Description>
 * This program is for testing the maximum number of
 * responses per with two different responses.
 * 
 */
 
byte Response = 2;
byte Response_2 = 4;

void setup()
{
  Serial.begin(9600);
  pinMode(Response, INPUT_PULLUP);
  pinMode(Response_2, INPUT_PULLUP);
}

void loop()
{
  Serial.println(String(digitalRead(Response))+String(digitalRead(Response_2)));
  delay(6);
}

