#include <WiFi.h>
#include "AsyncUDP.h"
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>

#include <SPI.h>
#include "EEPROM.h"
#include <Ticker.h>

#include "hidjoystickrptparser.h"
#include "ifparser.h"

#define ListenUdpPort 15000  // local port to listen on

#define WIFI_LED 32
#define CONNECT_LED 33
#define BUTTON_PIN 26

#define EEPROM_SIZE 8

AsyncUDP udp;
AsyncClient client;
Ticker ticker;

bool ConnectFlag = 0;

struct IFClient {  //存储客户端IP+端口
  IPAddress IP;
  uint16_t Port;
  bool updated = false;
} ClientAddr;

DynamicJsonDocument doc;

void SaveClientAddr(IFClient addr) {  //保存上次客户端地址到EEPROM
  int address = 0;
  EEPROM.writeUInt(address, uint32_t(addr.IP));
  address += sizeof(uint32_t(addr.IP));
  EEPROM.writeUShort(address, addr.Port);
  EEPROM.commit();
}

bool LoadClientAddr(IFClient& cli) { //读取上次客户端地址到EEPROM
  int address = 0;
  uint32_t ip = EEPROM.readUInt(address);
  address += sizeof(ip);
  uint16_t port = EEPROM.readUShort(address);

  if (ip & port) {

    cli.IP = ip;
    cli.Port = port;
    return true;
  } else {
    return false;
  }

}

void blinkLED() { //切换LED状态
  static bool WifiLedState = 0;
  digitalWrite(WIFI_LED, WifiLedState);
  WifiLedState = !WifiLedState;
}

void configModeCallback (WiFiManager *myWiFiManager) {

  ticker.attach(0.2, blinkLED);
}

bool ConnectClient() {  //从UDP或者EEPROM连接客户端

  if (ClientAddr.updated) {

    client.connect(ClientAddr.IP, ClientAddr.Port);
    if (client.connected()) {
      ClientAddr.IP.printTo(Serial);
      Serial.println("Connected.From UDP");
      ClientAddr.updated = false;
      SaveClientAddr(ClientAddr);
      return true;
    }

  }

  IFClient lastClient;
  if (LoadClientAddr(lastClient)) {
    lastClient.IP.printTo(Serial);
    Serial.println(lastClient.Port);
    client.connect(lastClient.IP, lastClient.Port);
    if (client.connected()) {
      Serial.print("Connected.From EEPROM");
      lastClient.IP.printTo(Serial);
      Serial.println(lastClient.Port);
      return true;
    }

  }

  return false;

}

void onData(void* obj, AsyncClient* c, void *data, size_t len) {
  if (len > 4) {
    ParseRecivedData(doc, (uint8_t*)data, len);
  }
}

void onConnect(void* obj, AsyncClient* c) {
  Serial.println("TCP Connected..");
  ConnectFlag = 1;
}

void setup()
{
  Serial.begin(2000000);
  Serial.println();

  pinMode(WIFI_LED, OUTPUT);
  //digitalWrite(WIFI_LED, 1);
  ticker.attach(0.6, blinkLED);
  pinMode(CONNECT_LED, OUTPUT);
  digitalWrite(CONNECT_LED, 1);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //////////////////////////////////////////////////
  Serial.printf("Connecting to Wifi.");
  //////////////////////////////////////////////////
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("InfiniteController");
  Serial.println("connected...yeey :)");
  ticker.detach();

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
  Serial.println("Initalize UDP Listen");
  //////////////////////////////////////////////////
  if (udp.listen(ListenUdpPort)) {
    Serial.println("UDP listening");
    udp.onPacket([](AsyncUDPPacket packet) {

      //Serial.write(packet.data(), packet.length());
      auto err = deserializeJson(doc, packet.data(), packet.length());

      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
        return;
      }

      JsonObject obj = doc.as<JsonObject>();

      JsonArray arr = obj["Addresses"];

      for (int i = 0; i < arr.size(); i++) {

        if (ClientAddr.IP.fromString(arr[i].as<String>())) {

          ClientAddr.Port = obj["Port"];
          ClientAddr.updated = true;
          return;

        }
      }
    });
  }
  //////////////////////////////////////////////////
  Serial.println("Initalize TCP Listen");
  //////////////////////////////////////////////////
  client.onConnect(onConnect);     //on successful connect
  client.onData(onData);           //data received
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
  digitalWrite(CONNECT_LED, 1);
  while (!ConnectFlag) {

    ConnectClient();

  }
  digitalWrite(CONNECT_LED, 0);

  for (;;) {

    //Realtime Task: Connection test
    if (!client.connected()) {
      Serial.println("disconnected.");
      ConnectFlag = 0;
      break;
    }

    //ReceiveClientData();

    SendCommandToClient("Airplane.Getstate");
    delay(500);
  }

}
