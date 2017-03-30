/*
 * Copyright: Toshikazu Kuroda
 * 
 * <Description>
 * This program is for testing the maximum number of
 * responses per sec with two different responses.
 * This program was made by modifying that originally was
 * programmed by Rogelio Escobar.
 * 
 */

int StartButton = 2;
int Response = 8;
int Response_2 = 7;

void setup()
{
  Serial.begin(9600);
  pinMode(StartButton, INPUT_PULLUP);
  pinMode(Response, OUTPUT);
  pinMode(Response_2, OUTPUT);
  digitalWrite(Response, LOW);
  digitalWrite(Response_2, LOW);
  Serial.println("Start");
}

void loop()
{
  if (digitalRead(StartButton) == LOW)
  {
    int i = 0;
    for(i = 1; i <= 10; i++)
    {      
      int j = 0;
      for(j = 1; j <= 500; j++)
      {
        delay(12); // IRT
        digitalWrite(Response, HIGH);
        delay(13);  // Response duration
        digitalWrite(Response, LOW);

        delay(12); // IRT
        digitalWrite(Response_2, HIGH);
        delay(13);  // Response duration
        digitalWrite(Response_2, LOW); 
      }
      delay(1000); // ITI
    }
    while(1)
    {
      Serial.println("End");
      delay(1000);
    }
  }
}
