#include  <Servo.h>

const int PIN_SERVO = 9;
const int PIN_LPWM = 11;
const int PIN_RPWM = 10;
const int PIN_LEN = 13;
const int PIN_REN = 12;

const int DIRECTION_AMPLITUDE = 30;
const int SPEED_AMPLITUDE = 70;
//const int SPEED_AMPLITUDE = 90;

Servo servo;

void setup()
{
  servo.attach(PIN_SERVO);
  pinMode(PIN_LPWM, OUTPUT);
  pinMode(PIN_RPWM, OUTPUT);
  pinMode(PIN_LEN, OUTPUT);
  pinMode(PIN_REN, OUTPUT);

  servo.write(90);
  digitalWrite(PIN_LEN, HIGH);
  digitalWrite(PIN_REN, HIGH);
  digitalWrite(PIN_LPWM, LOW);
  digitalWrite(PIN_RPWM, LOW);

  Serial.begin(9600);
}

void loop()
{
  String s = Serial.readStringUntil('\n');

  if(s.length() >= 2)
  {
    bool ok = false;
    
    if(s[0] == 'R')
    {
      digitalWrite(PIN_LEN, HIGH);
      digitalWrite(PIN_REN, HIGH);
      analogWrite(PIN_LPWM, SPEED_AMPLITUDE);
      analogWrite(PIN_RPWM, 0);
      ok = true;
    }
    else if(s[0] == 'S')
    {
      digitalWrite(PIN_LEN, LOW);
      digitalWrite(PIN_REN, LOW);
      analogWrite(PIN_LPWM, 0);
      analogWrite(PIN_RPWM, 0);
      ok = true;
    }
    
    if(ok)
    {
      double steering = atof(s.substring(1).c_str()) * 180.0/PI;
      steering = max(steering, -DIRECTION_AMPLITUDE);
      steering = min(steering, DIRECTION_AMPLITUDE);
  
      servo.write(90 + steering);
    }
  }
}
