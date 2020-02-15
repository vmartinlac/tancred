
#include  <Servo.h>

int PIN_LEFT_RIGHT = 9;
int PIN_LSPEED = 10;
int PIN_RSPEED = 11;

Servo servo;
int count = 0;

void setup()
{
  pinMode(PIN_LSPEED, OUTPUT);
  pinMode(PIN_RSPEED, OUTPUT);
  servo.attach(PIN_LEFT_RIGHT);

  digitalWrite(PIN_LSPEED, LOW);
  digitalWrite(PIN_RSPEED, LOW);
  servo.write(90);

  count = 0;
}

void loop()
{
  const int pause_ms = 5;

  const int period_ms = 1000;
  const int period_prop_ms = 5000;
  const float magnitude = 30;

  const float gamma = cos(3.1415f*float(count)*float(pause_ms)/float(period_ms));
  const float gamma_prop = cos(3.1415f*float(count)*float(pause_ms)/float(period_prop_ms));
  servo.write(90 + 30*gamma);

  if(gamma_prop > 0.0)
  {
    analogWrite(PIN_LSPEED, 230);
    analogWrite(PIN_RSPEED, 0);
  }
  else
  {
    analogWrite(PIN_LSPEED, 0);
    analogWrite(PIN_RSPEED, 230);
  }
  
  /*
  if( gamma_prop > 0.0 )
  {
    digitalWrite(PIN_FORWARD_BACKWARD_0, HIGH);
    digitalWrite(PIN_FORWARD_BACKWARD_1, LOW);
  }
  else
  {
    digitalWrite(PIN_FORWARD_BACKWARD_0, LOW);
    digitalWrite(PIN_FORWARD_BACKWARD_1, HIGH);
  }
  */

  count++;
  delay(pause_ms);
}
