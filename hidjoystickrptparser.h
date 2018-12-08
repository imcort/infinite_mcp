#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include <usbhid.h>

#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>

extern AsyncClient client;
extern DynamicJsonDocument doc;
//
//typedef struct{
//        uint16_t X, Y;
//        uint8_t Z, T;
//}GamePadEventData;
//
//class JoystickEvents {
//public:
//        virtual void OnGamePadChanged(const GamePadEventData *evt);
//        virtual void OnHatSwitch(uint8_t hat);
//        virtual void OnButtonUp(uint8_t but_id);
//        virtual void OnButtonDn(uint8_t but_id);
//};
//
//#define KEYNUM   12
//
//class JoystickReportParser : public HIDReportParser {
//        JoystickEvents *joyEvents;
//
//        GamePadEventData oldPad;
//        uint8_t oldHat;
//        uint16_t oldButtons;
//
//public:
//        JoystickReportParser(JoystickEvents *evt);
//
//        virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
//};

void SendCommandToClient(String Cmd/*, APICommand Cmd*/);
void SendJoystickToClient(uint8_t Joyname, int16_t Joyvalue);
void SendPOVToClient(int8_t xValue, int8_t yValue);
void SendButtonToClient(uint8_t btnNum, bool isPress);

#endif // __HIDJOYSTICKRPTPARSER_H__
