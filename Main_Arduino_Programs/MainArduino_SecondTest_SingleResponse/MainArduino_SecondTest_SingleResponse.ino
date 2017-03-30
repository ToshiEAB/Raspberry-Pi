/*
 * Copyright: Toshikazu
 *
 * <Description>
 * This program is for testing the maximum number of
 * responses per with a single response.
 * 
 */
 
byte Response = 2;

void setup()
{
  Serial.begin(9600);
  pinMode(Response, INPUT_PULLUP);
}

void loop()
{
  Serial.println(digitalRead(Response));
  delay(4);
}

