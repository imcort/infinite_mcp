#include <WiFi.h>
#include "AsyncUDP.h"
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

//#include <ArduinoJson.h>

#include <SPI.h>
#include "EEPROM.h"
#include <Ticker.h>

#include "ifparser.h"

#define ListenUdpPort 15000  // local port to listen on

//#define WIFI_LED 32
//#define CONNECT_LED 33
//#define BUTTON_PIN 26

#define EEPROM_SIZE 8

#include <TM1638lite.h>
#include <TFT_eSPI.h> // Hardware-specific library

AsyncUDP udp;
AsyncClient client;
Ticker ticker;

TM1638lite tm(26, 32, 33);
TM1638lite tm1(25, 32, 33);

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

bool ConnectFlag = 0;

void blinkLED() { //切换LED状态
  static bool WifiLedState = 0;
//  digitalWrite(WIFI_LED, WifiLedState);
  WifiLedState = !WifiLedState;
}

void configModeCallback (WiFiManager *myWiFiManager) {

  ticker.attach(0.2, blinkLED);
}

void RefreshLCD() {
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  String test = "AccX";
  tft.drawString(test+CurrentAirplane.AccelerationX, 0, 0, 2);
  test = "AccY";
  tft.drawString(test+CurrentAirplane.AccelerationY, 0, 15, 2);
  test = "AccZ";
  tft.drawString(test+CurrentAirplane.AccelerationZ, 0, 30, 2);
  test = "AltiAGL";
  tft.drawString(test+CurrentAirplane.AltitudeAGL, 0, 45, 2);
  test = "AltiMSL";
  tft.drawString(test+CurrentAirplane.AltitudeMSL, 0, 60, 2);
  test = "AICAO";
  tft.drawString(test+CurrentAirplane.ApproachAirportICAO, 0, 75, 2);
  test = "ADistance";
  tft.drawString(test+CurrentAirplane.ApproachDistance, 0, 90, 2);
  test = "AHAngle";
  tft.drawString(test+CurrentAirplane.ApproachHorizontalAngle, 0, 105, 2);
  test = "ARunway";
  tft.drawString(test+CurrentAirplane.ApproachRunway, 0, 120, 2);
  test = "AVAngle";
  tft.drawString(test+CurrentAirplane.ApproachVerticalAngle, 0, 135, 2);
  test = "Bank";
  tft.drawString(test+CurrentAirplane.Bank, 0, 150, 2);
  test = "Course";
  tft.drawString(test+CurrentAirplane.CourseTrue, 0, 165, 2);
  test = "Flaps";
  tft.drawString(test+CurrentAirplane.FlapsIndex, 0, 180, 2);
  test = "GForce";
  tft.drawString(test+CurrentAirplane.GForce, 0, 195, 2);
  test = "Gear";
  tft.drawString(test+CurrentAirplane.GearState, 0, 210, 2);
  test = "GSpd";
  tft.drawString(test+CurrentAirplane.GroundSpeed, 0, 225, 2);

  test = "GSpdK";
  tft.drawString(test+CurrentAirplane.GroundSpeedKts, 110, 0, 2);
  test = "HeadM";
  tft.drawString(test+CurrentAirplane.HeadingMagnetic, 110, 15, 2);
  test = "HeadT";
  tft.drawString(test+CurrentAirplane.HeadingTrue, 110, 30, 2);
  test = "IndAirspd";
  tft.drawString(test+CurrentAirplane.IndicatedAirspeed, 110, 45, 2);
  test = "IndAirspdK";
  tft.drawString(test+CurrentAirplane.IndicatedAirspeedKts, 110, 60, 2);
  test = "EngineCount";
  tft.drawString(test+CurrentAirplane.EngineCount, 110, 75, 2);
  test = "FlapsCount";
  tft.drawString(test+CurrentAirplane.FlapsCount, 110, 90, 2);
  test = "FuelTank";
  tft.drawString(test+CurrentAirplane.FuelTankCount, 110, 105, 2);
  test = "HasAP";
  tft.drawString(test+CurrentAirplane.HasAutopilot, 110, 120, 2);
  test = "AirName";
  tft.drawString(test+CurrentAirplane.AircraftName, 110, 135, 2);
  test = "SpoilType";
  tft.drawString(test+CurrentAirplane.SpoilerType, 110, 150, 2);
  test = "AppState";
  tft.drawString(test+CurrentAirplane.AppState, 110, 165, 2);
  test = "ApiVer";
  tft.drawString(test+CurrentAirplane.ApiVersion, 110, 180, 2);
  test = "DHeight";
  tft.drawString(test+CurrentAirplane.DisplayHeight, 110, 195, 2);
  test = "DWidth";
  tft.drawString(test+CurrentAirplane.DisplayWidth, 110, 210, 2);
  test = "Latitude";
  tft.drawString(test+CurrentAirplane.Latitude, 110, 225, 2);

  test = "Longitude";
  tft.drawString(test+CurrentAirplane.Longitude, 220, 0, 2);
  test = "MachNumber";
  tft.drawString(test+CurrentAirplane.MachNumber, 220, 15, 2);
  test = "MagneticDeviation";
  tft.drawString(test+CurrentAirplane.MagneticDeviation, 220, 30, 2);
  test = "Pitch";
  tft.drawString(test+CurrentAirplane.Pitch, 220, 45, 2);
  test = "ReverseThrustState";
  tft.drawString(test+CurrentAirplane.ReverseThrustState, 220, 60, 2);
  test = "SideForce";
  tft.drawString(test+CurrentAirplane.SideForce, 220, 75, 2);
  test = "SpoilersPosition";
  tft.drawString(test+CurrentAirplane.SpoilersPosition, 220, 90, 2);
  test = "StallProximity";
  tft.drawString(test+CurrentAirplane.StallProximity, 220, 105, 2);
  test = "StallWarning";
  tft.drawString(test+CurrentAirplane.StallWarning, 220, 120, 2);
  test = "Stalling";
  tft.drawString(test+CurrentAirplane.Stalling, 220, 135, 2);
  test = "TrueAirspeed";
  tft.drawString(test+CurrentAirplane.TrueAirspeed, 220, 150, 2);
  test = "Velocity";
  tft.drawString(test+CurrentAirplane.Velocity, 220, 165, 2);
  test = "VerticalSpeed";
  tft.drawString(test+CurrentAirplane.VerticalSpeed, 220, 180, 2);
  test = "Weight";
  tft.drawString(test+CurrentAirplane.Weight, 220, 195, 2);
  test = "WeightPercentage";
  tft.drawString(test+CurrentAirplane.WeightPercentage, 220, 210, 2);
  test = "DeviceName";
  tft.drawString(test+CurrentAirplane.DeviceName, 220, 225, 2);

}

void setup()
{
  Serial.begin(2000000);
  Serial.println();

//  pinMode(WIFI_LED, OUTPUT);
  //digitalWrite(WIFI_LED, 1);
//  ticker.attach(0.6, blinkLED);
//  pinMode(CONNECT_LED, OUTPUT);
//  digitalWrite(CONNECT_LED, 1);
//  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //////////////////////////////////////////////////
  Serial.printf("Connecting to Wifi.");
  //////////////////////////////////////////////////
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("InfiniteController");
  Serial.println("connected...yeey :)");
//  ticker.detach();

  //////////////////////////////////////////////////
  Serial.println("Initalize EEPROM");
  //////////////////////////////////////////////////
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  //////////////////////////////////////////////////
  Serial.println("Initalize LED Digit");
  //////////////////////////////////////////////////

  tm.reset();
  tm1.reset();

  //////////////////////////////////////////////////
  Serial.println("Initalize LCD");
  //////////////////////////////////////////////////

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  //////////////////////////////////////////////////
  Serial.println("Initalize UDP Listen");
  //////////////////////////////////////////////////
  if (udp.listen(ListenUdpPort)) {
    Serial.println("UDP listening");
    udp.onPacket([](AsyncUDPPacket packet) {
      size_t len = packet.length();
      ParseUDPRecivedData(packet.data(), len);

    });
  }
  //////////////////////////////////////////////////
  Serial.println("Initalize TCP Listen");
  //////////////////////////////////////////////////
  client.onConnect([](void* obj, AsyncClient * c) {
    Serial.println("TCP Connected..");
    ConnectFlag = 1;
  });     //on successful connect
  client.onData([](void* obj, AsyncClient * c, void *data, size_t len) {
    if (len > 4) {
      ParseTCPRecivedData((uint8_t*)data, len);
    }
  });           //data received
  /*
    client.onConnect(onConnect);     //on successful connect
    client.onDisconnect(onDisconnect);  //disconnected
    client.onAck(onAck);             //ack received
    client.onError(onError);         //unsuccessful connect or error
    client.onData(onData);           //data received
    client.onTimeout(onTimeout);     //ack timeout
    client.onPoll(onPoll);        //every 125ms when connected
  */
}

unsigned long timer = 0;

void loop() {
//  digitalWrite(CONNECT_LED, 1);
  while (!ConnectFlag) {

    ConnectClient();
    delay(500);
  }
//  digitalWrite(CONNECT_LED, 0);
  for (;;) {

    //Realtime Task: Connection test
    if (!client.connected()) {
      Serial.println("disconnected.");
      ConnectFlag = 0;
      break;
    }

    SendCommandToClient("Airplane.Getstate");
    delay(100);
    SendCommandToClient("Airplane.GetInfo");
    delay(100);
    SendCommandToClient("InfiniteFlight.GetStatus");
    delay(100);

    tm.displayNumber(1, (int32_t)CurrentAirplane.Velocity);
    tm1.displayNumber(1, (int32_t)CurrentAirplane.AltitudeLight);

    RefreshLCD();
    
  }

}
