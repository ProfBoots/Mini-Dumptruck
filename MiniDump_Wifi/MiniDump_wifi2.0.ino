// make sure to upload with ESP32 Dev Module selected as the board under tools>Board>ESP32 Arduino

#include <Arduino.h>

#include <ESP32Servo.h> // by Kevin Harrington
#include <ESPAsyncWebSrv.h> // by dvarrel
#include <iostream>
#include <sstream>

#if defined(ESP32)
#include <AsyncTCP.h> // by dvarrel
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h> // by dvarrel
#endif

// defines

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

// global constants

extern const char* htmlHomePage PROGMEM;
const char* ssid = "MegaDump";
 
// global variables

Servo steeringServo;
Servo auxServo;

int steeringServoValue = 86;
int steeringTrimValue = 0;
int auxServoValue = 90;
int lightSwitchTime = 0;
bool horizontalScreen; // when screen orientation is locked vertically this rotates the D-Pad controls so that forward would now be left.
bool lightsOn = false;

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

void steeringControl(int steeringValue)
{
  steeringServoValue = steeringValue;
  steeringServo.write(steeringServoValue + steeringTrimValue);
}

void auxControl(int auxServoValue)
{
    auxServo.write(auxServoValue);
}

void dumpControl(int dumpValue){
if (dumpValue == 5) {
    digitalWrite(auxAttach2, HIGH);
    digitalWrite(auxAttach3, LOW);
  } else if (dumpValue == 6) {
    digitalWrite(auxAttach2, LOW);
    digitalWrite(auxAttach3, HIGH);
  } else {
    digitalWrite(auxAttach2, LOW);
    digitalWrite(auxAttach3, LOW);
  }
}
void moveMotor(int motorPin1, int motorPin0, int velocity) {
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
void lightControl()
{
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
}
void steeringTrim(int steeringTrim)
{
  if(steeringTrim == 1 && steeringTrimValue < 40)
  {
    steeringTrimValue = steeringTrimValue + 2;
  }
  else if(steeringTrimValue > -20)
  {
    steeringTrimValue = steeringTrimValue - 2;
  }
steeringControl(steeringServoValue);
}

void handleRoot(AsyncWebServerRequest *request)
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server,
                              AsyncWebSocketClient *client,
                              AwsEventType type,
                              void *arg,
                              uint8_t *data,
                              size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      //Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str());
        int valueInt = atoi(value.c_str());
        if (key == "steering")
        {
          steeringControl(valueInt);
        }
        else if (key == "throttle")
        {
          moveMotor(leftMotor0, leftMotor1, valueInt);
          moveMotor(rightMotor0, rightMotor1, valueInt);
        }
        else if (key == "dump")
        {
          dumpControl(valueInt);
        }
        else if (key == "light")
        {
          lightControl();
        }
        else if (key == "trim")
        {
          steeringTrim(valueInt);
        }
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;
  }
}

void setUpPinModes()
{
  pinMode(auxAttach2, OUTPUT);
  pinMode(auxAttach3, OUTPUT);
  pinMode(auxAttach0, OUTPUT);
  pinMode(auxAttach1, OUTPUT);
  pinMode(leftMotor0, OUTPUT);
  pinMode(leftMotor1, OUTPUT);
  pinMode(rightMotor0, OUTPUT);
  pinMode(rightMotor1, OUTPUT);

  digitalWrite(auxAttach2, LOW);
  digitalWrite(auxAttach3, LOW);

  steeringServo.attach(steeringServoPin);
  auxServo.attach(auxServoPin);
  steeringControl(steeringServoValue);
  auxControl(auxServoValue);
}


void setup(void)
{
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid );
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  //Serial.println("HTTP server started");
}

void loop()
{
  wsCarInput.cleanupClients();
}
