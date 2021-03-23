#include "HardwareSerial_NB_BC95.h"

#include <ArduinoJson.h>

String deviceToken = "x6KaNi7sVeIkr80LEESE";
String serverIP = "103.27.203.83"; // Your Server IP;
String serverPort = "19956"; // Your Server Port;
String json = "";

int _second = 0;
String _millisStr = "";
String millisStr = "";


HardwareSerial_NB_BC95 AISnb;

const long interval = 20000;  //millisecond
unsigned long previousMillis = 0;
String udpData = "{\"_type\":\"retrattr\",\"Tn\":\"dwt9m5mkNAc3jqCVzxJH\",\"keys\":[\"epoch\",\"ip\"]}";

unsigned long _epoch = 0;
String _IP = "";


//String udpData3 {\"Tn\":\"dwt9m5mkNAc3jqCVzxJH\","key1":9, "key2":9}";
String dataJson = "";
StaticJsonDocument<400> doc;
boolean validEpoc = false;
void setup()
{


  AISnb.debug = false;

  Serial.begin(115200);
  Serial.println("Start...");

  AISnb.setupDevice(serverPort);

  String ip1 = AISnb.getDeviceIP();
  delay(1000);

  pingRESP pingR = AISnb.pingIP(serverIP);
  previousMillis = millis();
  _init();
}


char  char_to_byte(char c)
{
  if ((c >= '0') && (c <= '9'))
  {
    return (c - 0x30);
  }
  if ((c >= 'A') && (c <= 'F'))
  {
    return (c - 55);
  }
}

void _init() {

  Serial.println(udpData);

  do {
    UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, udpData);

    String nccid = AISnb.getNCCID();
    Serial.print("nccid:");
    Serial.println(nccid);


    UDPReceive resp = AISnb.waitResponse();
    AISnb.receive_UDP(resp);
    Serial.print("waitData:");
    Serial.println(resp.data);


    for (int x = 0; x < resp.data.length(); x += 2)
    {
      char c =  char_to_byte(resp.data[x]) << 4 | char_to_byte(resp.data[x + 1]);

      dataJson += c;
    }
    Serial.println(dataJson);
    DeserializationError error = deserializeJson(doc, dataJson);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      validEpoc = true;
      delay(4000);
    } else {
      validEpoc = false;
      unsigned long epoch = doc["epoch"];
      _epoch = epoch;
      String ip = doc["ip"];
      _IP = ip;
      //      Serial.println(dataJson);
      Serial.print("epoch:");  Serial.println(_epoch);
      Serial.println(_IP);

    }



    //
  } while (validEpoc);

  //
  //    // Print values.

  //    Serial.println(pm25_value);
  //    Serial.println(pm10_value);

}
void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    dataJson = "";
    String udpData2 = "{\"Tn\":\"dwt9m5mkNAc3jqCVzxJH\",\"ts\":";
    String udpData3 = ", \"values\":{\"key1\":6, \"key2\":6}}";

    _millisStr = millis();
    millisStr = _millisStr.substring(_millisStr.length() - 3, _millisStr.length());
    _second = millis() / 1000;

    _epoch += _second;
    udpData2.concat(_epoch);
    udpData2.concat(millisStr);
    udpData2.concat(udpData3);
    Serial.println(udpData2);

    UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, udpData2);

    previousMillis = currentMillis;
    //    String nccid = AISnb.getNCCID();
    //    Serial.print("nccid:");
    //    Serial.println(nccid);

    UDPReceive resp = AISnb.waitResponse();
    AISnb.receive_UDP(resp);
    Serial.print("waitData:");
    Serial.println(resp.data);


    previousMillis = currentMillis;
  }
}
