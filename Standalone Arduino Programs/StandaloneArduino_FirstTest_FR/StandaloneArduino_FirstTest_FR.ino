/*
 * Copyright: Toshikazu Kuroda
 * 
 * <Description>
 * This program is for testing the latency from a response
 * to the reinforcer it produces in an FR schedule.
 * This program was made by modifying that originally was
 * programmed by Rogelio Escobar.
 * 
 */


unsigned long start, finished, elapsed;
int Status = 1;
int StartButton = 2;
int Reinforcer = 3;
int Response = 8;

void setup()
{
  Serial.begin(9600);
  pinMode(StartButton, INPUT_PULLUP);
  pinMode(Reinforcer, INPUT_PULLUP);
  pinMode(Response, OUTPUT); //
  digitalWrite(Response, LOW);
  Serial.println("Start");
}

void displayResult()
{
  elapsed = finished - start;
  Serial.println(elapsed);
  digitalWrite(Response, LOW); 
  delay(1000); // ITI
}

void loop()
{
  if (digitalRead(StartButton) == LOW && Status ==1)
  {
    start = micros();
    digitalWrite(Response, HIGH);
    Status = 2;
  }
  
  if (digitalRead(Reinforcer) == LOW && Status == 2)
  {
    finished = micros();
    Status = 1;
    displayResult();
  }
}
