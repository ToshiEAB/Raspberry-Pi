/*
 * Copyright: Toshikazu Kuroda
 * 
 * <Description>
 * This program is for testing the latency from a response
 * to the reinforcer it produces in an concurrent VI VI schedule.
 * This program was made by modifying that originally was
 * programmed by Rogelio Escobar.
 * 
 */

unsigned long start, finished, elapsed;
int Status = 1;
int Flag = 1;
int StartButton = 2;
int Reinforcer = 3;
int Response = 8;
int Response2 = 7;

void setup()
{
  Serial.begin(9600);
  pinMode(StartButton, INPUT_PULLUP);
  pinMode(Reinforcer, INPUT_PULLUP);
  pinMode(Response, OUTPUT); 
  pinMode(Response2, OUTPUT); 
  digitalWrite(Response, LOW);
  digitalWrite(Response2, LOW);
  Serial.println("Start");
}

void displayResult()
{
  elapsed = finished - start;
  Serial.println(elapsed);  
  delay(1000); // ITI
}

void loop()
{    
  while (digitalRead(StartButton) == LOW)
  {
      while (Flag == 1)
      {
        if (Status ==1)
        {
          start = micros();
          digitalWrite(Response, HIGH);
          Status = 2;
        }
    
        if (digitalRead(Reinforcer) == LOW && Status == 2)
        {
          finished = micros();
          digitalWrite(Response, LOW); // New
          Status = 3;
          Flag = 2;
          displayResult();
        }

        if (micros() - start > 100000) // 100 ms
        {
          digitalWrite(Response, LOW);
          Status = 3;
          Flag = 2;
        }
      }
    
    while (Flag == 2)
    {
        if (Status ==3)
        {
          start = micros();
          digitalWrite(Response2, HIGH);
          Status = 4;
        }
    
        if (digitalRead(Reinforcer) == LOW && Status == 4)
        {
          finished = micros();
          digitalWrite(Response2, LOW); // New
          Status = 1;
          Flag = 1;
          displayResult();
        }

        if (micros() - start > 100000) // 100 ms
        {
          digitalWrite(Response2, LOW);
          Status = 1;
          Flag = 1;
        }
    }
  }
}
