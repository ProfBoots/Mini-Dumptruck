#include <Ps3Controller.h>
#include <ESP32Servo.h>  // by Kevin Harrington

#define steeringServoPin 23
#define auxServoPin 22
#define cabLights 32
#define auxLights 33

#define auxAttach0 25  // Used for controlling auxiliary attachment movement
#define auxAttach1 26  // Used for controlling auxiliary attachment movement
#define auxAttach2 18  // Used for controlling auxiliary attachment movement
#define auxAttach3 17  // Used for controlling auxiliary attachment movement

#define leftMotor0 33   // Used for controlling the left motor movement
#define leftMotor1 32   // Used for controlling the left motor movement
#define rightMotor0 21  // Used for controlling the right motor movement
#define rightMotor1 19  // Used for controlling the right motor movement

Servo steeringServo;
Servo auxServo;
int lightSwitchTime = 0;
int adjustedSteeringValue = 86;
int auxServoValue = 90;
bool lightsOn = false;


void notify() {
  //--------------- Digital D-pad button events --------------
  if (Ps3.event.button_down.up) {
    Serial.println("Started pressing the up button");
    digitalWrite(auxAttach2, HIGH);
    digitalWrite(auxAttach3, LOW);
  }
  if (Ps3.event.button_up.up) {
    Serial.println("Released the up button");
    digitalWrite(auxAttach2, LOW);
    digitalWrite(auxAttach3, LOW);
  }
  if (Ps3.event.button_down.down) {
    Serial.println("Started pressing the down button");
    digitalWrite(auxAttach2, LOW);
    digitalWrite(auxAttach3, HIGH);
  }
  if (Ps3.event.button_up.down) {
    Serial.println("Released the down button");
    digitalWrite(auxAttach2, LOW);
    digitalWrite(auxAttach3, LOW);
  }

//---------------- Analog stick value events ---------------
if (abs(Ps3.event.analog_changed.stick.lx) + abs(Ps3.event.analog_changed.stick.ly) > 2) {
  Serial.print("Moved the left stick:");
  Serial.print(" x=");
  Serial.print(Ps3.data.analog.stick.lx, DEC);
  Serial.print(" y=");
  Serial.print(Ps3.data.analog.stick.ly, DEC);
  Serial.println();
  int LYValue = Ps3.data.analog.stick.ly;
  int adjustedThrottleValue = LYValue * 2;
  moveMotor(leftMotor0, leftMotor1, adjustedThrottleValue);
  moveMotor(rightMotor0, rightMotor1, adjustedThrottleValue);
}

if (abs(Ps3.event.analog_changed.stick.rx) + abs(Ps3.event.analog_changed.stick.ry) > 2) {
  Serial.print("Moved the right stick:");
  Serial.print(" x=");
  Serial.print(Ps3.data.analog.stick.rx, DEC);
  Serial.print(" y=");
  Serial.print(Ps3.data.analog.stick.ry, DEC);
  Serial.println();
  int RXValue = (Ps3.data.analog.stick.rx);

  adjustedSteeringValue = 90 - (RXValue / 3);
  steeringServo.write(adjustedSteeringValue);
}
if (Ps3.event.button_down.r3) {
  if ((millis() - lightSwitchTime) > 200) {
    if (lightsOn) {
      digitalWrite(auxAttach0, LOW);
      digitalWrite(auxAttach1, LOW);
      lightsOn = false;
    } else {
      digitalWrite(auxAttach0, HIGH);
      digitalWrite(auxAttach1, LOW);
      lightsOn = true;
    }

    lightSwitchTime = millis();
  }
  Serial.println("Started pressing the right stick button");
}
}

void moveMotor(int motorPin0, int motorPin1, int velocity) {
  if (velocity > 15) {
    analogWrite(motorPin0, velocity);
    analogWrite(motorPin1, LOW);
  } else if (velocity < -15) {
    analogWrite(motorPin0, LOW);
    analogWrite(motorPin1, (-1 * velocity));
  } else {
    analogWrite(motorPin0, 0);
    analogWrite(motorPin1, 0);
  }
}

void onConnect() {
  Serial.println("Connected.");
}

void setup() {

  Serial.begin(115200);

  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("8c:7c:b5:fc:3b:39");

  Serial.println("Ready.");

   pinMode(auxAttach2, OUTPUT);
  pinMode(auxAttach3, OUTPUT);
  pinMode(auxAttach0, OUTPUT);
  pinMode(auxAttach1, OUTPUT);
  digitalWrite(auxAttach2, LOW);
  digitalWrite(auxAttach3, LOW);
  pinMode(leftMotor0, OUTPUT);
  pinMode(leftMotor1, OUTPUT);
  pinMode(rightMotor0, OUTPUT);
  pinMode(rightMotor1, OUTPUT);


  steeringServo.attach(steeringServoPin);
  steeringServo.write(adjustedSteeringValue);
}
void loop() {
  if (!Ps3.isConnected())
    return;
  delay(500);
}
