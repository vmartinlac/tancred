#include  <Servo.h>

int PIN_SERVO = 9;
int PIN_LPWM = 11;
int PIN_RPWM = 10;
int PIN_LEN = 13;
int PIN_REN = 12;

int DIRECTION_AMPLITUDE = 30;
int SPEED_AMPLITUDE = 90;

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
  String s = Serial.readStringUntil(' ');

  if(s.equals("direction-left"))
  {
    servo.write(90 - DIRECTION_AMPLITUDE);
  }
  else if(s.equals("direction-front"))
  {
    servo.write(90);
  }
  else if(s.equals("direction-right"))
  {
    servo.write(90 + DIRECTION_AMPLITUDE);
  }
  else if(s.equals("speed-stop"))
  {
    digitalWrite(PIN_LEN, LOW);
    digitalWrite(PIN_REN, LOW);
    analogWrite(PIN_LPWM, 0);
    analogWrite(PIN_RPWM, 0);
  }
  else if(s.equals("speed-forward"))
  {
    digitalWrite(PIN_LEN, HIGH);
    digitalWrite(PIN_REN, HIGH);
    analogWrite(PIN_LPWM, SPEED_AMPLITUDE);
    analogWrite(PIN_RPWM, 0);
  }
  else if(s.equals("speed-backward"))
  {
    digitalWrite(PIN_LEN, HIGH);
    digitalWrite(PIN_REN, HIGH);
    analogWrite(PIN_LPWM, 0);
    analogWrite(PIN_RPWM, SPEED_AMPLITUDE);
  }
}
