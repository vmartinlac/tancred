#include  <Servo.h>

int MYPIND = 3;
int MYPINP1 = 4;
int MYPINP2 = 5;

Servo servo;
int count = 0;

void setup()
{
  pinMode(MYPINP1, OUTPUT);
  pinMode(MYPINP2, OUTPUT);
  servo.attach(MYPIND);
  
  digitalWrite(MYPINP1, HIGH);
  digitalWrite(MYPINP2, HIGH);
  servo.write(90);
  
  count = 0;
}

void loop()
{
  const int pause_ms = 5;
  const int period_ms = 1000;
  const int period_prop_ms = 10000;
  const float magnitude = 30;
  
  const float gamma = cos(3.1415f*float(count)*float(pause_ms)/float(period_ms));
  const float gamma_prop = cos(3.1415f*float(count)*float(pause_ms)/float(period_prop_ms));
  servo.write(90 + 30*gamma);
  if( gamma_prop > 0.0 )
  {
      digitalWrite(MYPINP1, HIGH);
      digitalWrite(MYPINP2, HIGH);
  }
  else
  {
      digitalWrite(MYPINP1, HIGH);
      digitalWrite(MYPINP2, HIGH);
  }
  
  count++;
  
  delay(pause_ms);
}
