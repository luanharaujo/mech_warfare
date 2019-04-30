#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(&Wire, 0x40);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  200 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  645 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 15;

#ifndef STASSID
#define STASSID "601D"
#define STAPSK  "men1nate"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  
  delay(10);
  
  //Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  ArduinoOTA.setPort(4242);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("MelhorRobozaoDeTodos");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
  });
  ArduinoOTA.onEnd([](){});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){});
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  
  pwm.setPWM(servonum, 0, 200);
  delay(1000);
  pwm.setPWM(servonum, 0, 300);
  delay(1000);
  pwm.setPWM(servonum, 0, 400);
  delay(1000);
  
}
