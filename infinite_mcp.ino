/*
 * Dependencies:
 * AsyncTCP:https://github.com/me-no-dev/AsyncTCP
 * MCPanel:
 * PCF8575:
 * TFT_eSPI:
 * WiFiManager-development:
 * ArduinoJson 6.8.0-beta:
 */

#include <WiFi.h>
#include "AsyncUDP.h"
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#include <SPI.h>
#include "EEPROM.h"
#include <Ticker.h>

#include "ifparser.h"

#define ListenUdpPort 15000  // local port to listen on

#define EEPROM_SIZE 8

#include <TFT_eSPI.h> // Hardware-specific library

#include <MCPanel.h>
#include "Wire.h"

AsyncUDP udp;
AsyncClient client;
Ticker SendCommandTicker, MakeConnectTicker;

MCPanel mcp;

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

TaskHandle_t Task1,Task2;

void SendCommandTask() { //切换LED状态

  static int i = 1;

  switch (i) {
    case 1:
      SendCommandToClient("Airplane.Getstate");
      i++;
      break;
    case 2:
      SendCommandToClient("Airplane.GetInfo");
      i++;
      break;
    case 3:
      SendCommandToClient("InfiniteFlight.GetStatus");
      i = 1;
      break;
  }
}

void MakeConnectTask() {

  Serial.println("connecting");
  ConnectClient();

}

void configModeCallback (WiFiManager *myWiFiManager) {

}

void RefreshLCD() {

  //tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  int screenX = 0;

  String test = "AcX:";
  //  tft.drawString(test + CurrentAirplane.AccelerationX, screenX, 0, 2);
  //  test = "AcY:";
  //  tft.drawString(test + CurrentAirplane.AccelerationY, screenX, 15, 2);
  //  test = "AcZ:";
  //  tft.drawString(test + CurrentAirplane.AccelerationZ, screenX, 30, 2);
  //  test = "AGL:";
  //  tft.drawString(test + CurrentAirplane.AltitudeAGL, screenX, 45, 2);
  test = "MSL:";
  tft.drawString(test + CurrentAirplane.AltitudeMSL, screenX, 60, 2);
  test = "ICAO:";
  tft.drawString(test + CurrentAirplane.ApproachAirportICAO, screenX, 75, 2);
  test = "ADST:";
  tft.drawString(test + CurrentAirplane.ApproachDistance, screenX, 90, 2);
  test = "AHA:";
  tft.drawString(test + CurrentAirplane.ApproachHorizontalAngle, screenX, 105, 2);
  test = "ARW:";
  tft.drawString(test + CurrentAirplane.ApproachRunway, screenX, 120, 2);
  test = "AVA:";
  tft.drawString(test + CurrentAirplane.ApproachVerticalAngle, screenX, 135, 2);
  test = "Bank:";
  tft.drawString(test + CurrentAirplane.Bank, screenX, 150, 2);
  //  test = "Course:";
  //  tft.drawString(test + CurrentAirplane.CourseTrue, screenX, 165, 2);
  test = "Flaps:";
  tft.drawString(test + CurrentAirplane.FlapsIndex, screenX, 180, 2);
  test = "GForce:";
  tft.drawString(test + CurrentAirplane.GForce, screenX, 195, 2);
  test = "Gear:";
  tft.drawString(test + CurrentAirplane.GearState, screenX, 210, 2);
  //  test = "GSpd:";
  //  tft.drawString(test + CurrentAirplane.GroundSpeed, screenX, 225, 2);

  screenX = 100;

  test = "GSpdK:";
  tft.drawString(test + CurrentAirplane.GroundSpeedKts, screenX, 0, 2);
  test = "HeadM:";
  tft.drawString(test + CurrentAirplane.HeadingMagnetic, screenX, 15, 2);
  //  test = "HeadT:";
  //  tft.drawString(test + CurrentAirplane.HeadingTrue, screenX, 30, 2);
  //  test = "IndAirspd:";
  //  tft.drawString(test + CurrentAirplane.IndicatedAirspeed, screenX, 45, 2);
  test = "AirspdK:";
  tft.drawString(test + CurrentAirplane.IndicatedAirspeedKts, screenX, 60, 2);
  //  test = "EngineCount:";
  //  tft.drawString(test + CurrentAirplane.EngineCount, screenX, 75, 2);
  test = "FlapsCount:";
  tft.drawString(test + CurrentAirplane.FlapsCount, screenX, 90, 2);
  //  test = "FuelTank:";
  //  tft.drawString(test + CurrentAirplane.FuelTankCount, screenX, 105, 2);
  test = "HasAP:";
  tft.drawString(test + CurrentAirplane.HasAutopilot, screenX, 120, 2);
  test = "";
  tft.drawString(test + CurrentAirplane.AircraftName, screenX, 135, 2);
  test = "SpoilType:";
  tft.drawString(test + CurrentAirplane.SpoilerType, screenX, 150, 2);
  test = "AppState:";
  tft.drawString(test + CurrentAirplane.AppState, screenX, 165, 2);
  test = "ApiVer:";
  tft.drawString(test + CurrentAirplane.ApiVersion, screenX, 180, 2);
  //  test = "DHeight:";
  //  tft.drawString(test + CurrentAirplane.DisplayHeight, screenX, 195, 2);
  //  test = "DWidth:";
  //  tft.drawString(test + CurrentAirplane.DisplayWidth, screenX, 210, 2);
  test = "Lat:";
  tft.drawString(test + CurrentAirplane.Latitude, screenX, 225, 2);

  screenX = 220;

  test = "Lon:";
  tft.drawString(test + CurrentAirplane.Longitude, screenX, 0, 2);
  test = "Mach:";
  tft.drawString(test + CurrentAirplane.MachNumber, screenX, 15, 2);
  //  test = "MD:";
  //  tft.drawString(test + CurrentAirplane.MagneticDeviation, screenX, 30, 2);
  test = "Pitch:";
  tft.drawString(test + CurrentAirplane.Pitch, screenX, 45, 2);
  test = "RTS:";
  tft.drawString(test + CurrentAirplane.ReverseThrustState, screenX, 60, 2);
  test = "SFe:";
  tft.drawString(test + CurrentAirplane.SideForce, screenX, 75, 2);
  test = "SpP:";
  tft.drawString(test + CurrentAirplane.SpoilersPosition, screenX, 90, 2);
  test = "SpT:";
  tft.drawString(test + CurrentAirplane.SpoilerType, screenX, 105, 2);
  test = "StallW:";
  tft.drawString(test + CurrentAirplane.StallWarning, screenX, 120, 2);
  test = "Stall:";
  tft.drawString(test + CurrentAirplane.Stalling, screenX, 135, 2);
  //  test = "TAs:";
  //  tft.drawString(test + CurrentAirplane.TrueAirspeed, screenX, 150, 2);
  //  test = "Spd:";
  //  tft.drawString(test + CurrentAirplane.Velocity, screenX, 165, 2);
  test = "Vs:";
  tft.drawString(test + CurrentAirplane.VerticalSpeed, screenX, 180, 2);
  //  test = "WEI:";
  //  tft.drawString(test + CurrentAirplane.Weight, screenX, 195, 2);
  //  test = "WPG:";
  //  tft.drawString(test + CurrentAirplane.WeightPercentage, screenX, 210, 2);
  test = "DNm:";
  tft.drawString(test + CurrentAirplane.DeviceName, screenX, 225, 2);

}
void func1(bool i, uint8_t j) {

  switch(i){
    case 0:
      Serial.print("Key Pressed:");
      break;
    case 1:
      Serial.print("Key Released:");
  }
  Serial.println(j + 1);
  mcp.displayNumber(0,j);
  
}

void func2(uint8_t i, int j) {
  Serial.print("Encoder ");
  Serial.print(i);
  Serial.print(" Changed:");
  Serial.println(j);
  mcp.displayNumber(i,j);
  if(i==0)
    SendAPToClient("SetHeading", j);
}

void codeForTask1( void * parameter )
{
  for (;;) {
    RefreshLCD();
  }
}

void codeForTask2( void * parameter )
{

  for (;;) {
    //mcp.displayNumber((int16_t)CurrentAirplane.AltitudeMSL, 
    //                  (int16_t)CurrentAirplane.IndicatedAirspeedKts, 
    //                  (int16_t)(CurrentAirplane.VerticalSpeed * 196.85), 
    //                  (int16_t)CurrentAirplane.HeadingMagnetic);
     mcp.changeCallbackFunc(func1, func2);
    
  }
}

void setup()
{
  Serial.begin(2000000);
  Serial.println();

  //////////////////////////////////////////////////
  Serial.printf("Connecting to Wifi.");
  //////////////////////////////////////////////////
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("InfiniteController");
  Serial.println("connected...yeey :)");

  //////////////////////////////////////////////////
  Serial.println("Initalize EEPROM");
  //////////////////////////////////////////////////
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  LoadClientAddr(CurrentAirplane.ClientAddress, CurrentAirplane.ClientPort);
  //Load Last client address EEPROM -> CurrentAirplane

  //////////////////////////////////////////////////
  Serial.println("Initalize MCPanel");
  //////////////////////////////////////////////////

  mcp.begin();
  xTaskCreatePinnedToCore(
    codeForTask2,            /* Task function. */
    "MCPTask",                 /* name of task. */
    5000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    2,                        /* priority of the task */
    &Task2,                   /* Task handle to keep track of created task */
    1);

  //////////////////////////////////////////////////
  Serial.println("Initalize LCD");
  //////////////////////////////////////////////////

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  xTaskCreatePinnedToCore(
    codeForTask1,            /* Task function. */
    "RefreshLCDTask",                 /* name of task. */
    5000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    1,                        /* priority of the task */
    &Task1,                   /* Task handle to keep track of created task */
    1);

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
    MakeConnectTicker.detach();
    SendCommandTicker.attach(0.1, SendCommandTask);
    SaveClientAddr(CurrentAirplane.ClientAddress, CurrentAirplane.ClientPort);

  });     //on successful connect
  client.onData([](void* obj, AsyncClient * c, void *data, size_t len) {

    if (len > 4) {
      ParseTCPRecivedData((uint8_t*)data, len);
    }

  });           //data received
  client.onDisconnect([](void* obj, AsyncClient * c) {

    SendCommandTicker.detach();
    MakeConnectTicker.attach(0.5, MakeConnectTask);
    Serial.print("onDisconnect");

  });
  MakeConnectTicker.attach(0.5, MakeConnectTask);
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

void loop() {
 
 delay(1000);

}
