#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define wifistate_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ssid_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a1"
#define pw_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a2"

volatile bool do1 = true;
volatile bool do2 = true;

BLECharacteristic *c_ssid = NULL;
BLECharacteristic *c_pw = NULL;
BLECharacteristic * c_wifistate = NULL;

String dosome = "none";

bool ble_on = false;
bool wifi_on = false;

String machineinfo = "";
String netcommand = "";
String machineid = "";


hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

String s2_command = "";

String mqttid_sim7020 = "0";
String mqttsvip = "94.191.14.111";
String mqttsvport = "2000";


//****forwifi*****
String ssid =  "DSCNAP";
String password = "DrinkStation88";
const char* mqttsv = "www.windcoffee.club";
uint16_t mqttport = 2000;
WiFiClient espClient;
PubSubClient client(espClient);


void openwifi()
{
  //Serial.println("open wifi");
  WiFi.begin(ssid.c_str(), password.c_str());
  delay(5000);

  if (WiFi.status() == WL_CONNECTED)
  {
    wifi_on = true;

    client.setServer(mqttsv, mqttport);
    client.setCallback(callback);
    
    //Serial.println("wifi opened");
            c_wifistate->setValue("ON");
        c_wifistate->notify();
     }else{
      //Serial.println("wifi close");
        c_wifistate->setValue("OFF");
        c_wifistate->notify(); 
     }
}

void mqttpub(String msg)
{
  byte   ahabyte[msg.length()];
  msg.getBytes(ahabyte, sizeof(ahabyte) + 1);

  char str[sizeof(ahabyte) * 2] = "";
  array_to_string(ahabyte, sizeof(ahabyte), str);

  Serial2.println("AT+CMQPUB=" + mqttid_sim7020  + ",\"DrankStation\",1,0,0," + (String)strlen(str)  +  ",\"" + str   + "\"");
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}

String bytestostring(String hex)
{
  String thes = "";
  unsigned char val[hex.length() / 2];
  for (size_t count = 0; count < sizeof val / sizeof * val; count++) {
    sscanf(hex.c_str() + 2 * count, "%2hhx", &val[count]);
    char  char1 = char( hex_to_ascii(hex[2 * count], hex[2 * count + 1]));
    thes +=  char1;
  }
  return thes;
}

int hex_to_int(char c) {
  int first = c / 16 - 3;
  int second = c % 16;
  int result = first * 10 + second;
  if (result > 9) result--;
  return result;
}

int hex_to_ascii(char c, char d) {
  int high = hex_to_int(c) * 16;
  int low = hex_to_int(d);
  return high + low;
}

void IRAM_ATTR  onTimer1() {
  portENTER_CRITICAL_ISR(&timerMux);

  if (do1)
  {
    do1 = false;
  } else if (do2)
  {
    do2 = false;
  } else {
    do1 = true;
    do2 = true;
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string newssid = c_ssid->getValue();
      std::string newpw = c_pw->getValue();

      ssid = newssid.c_str();
      password = newpw.c_str();
      dosome = "openwifi"; 
};
};

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.println("Message arrived ");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    if (client.connect(machineid.c_str())) {
      //Serial.println("connected");

 delay(2000);
    } else {
      //Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {

  Serial.begin(115200);
  delay(500);

  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  delay(500);

  timer1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 1000000, true);
  timerStart(timer1);
  timerAlarmEnable(timer1);
}

void loop() {

  if (wifi_on)
  {
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
  }

  if (dosome == "openwifi")
  {
    openwifi();
    dosome = "none";
  }

  if (dosome == "openble")
  {
    runBLE();
    dosome = "none";
  }

  if (dosome == "sendmqtt")
  {
    if (wifi_on)
    {
      //Serial.println("send mqtt by wifi");
      //Serial.println("send mqtt info");
      
      client.beginPublish("DrankStation", machineinfo.length(), false);
      client.print(machineinfo);
      client.endPublish();
      //Serial.println("send mqtt info ok");
      //Serial.println(machineinfo);
    } else
    {
  mqttid_sim7020 =  getmqttid();
  String myclient =  machineid + "SIM";
  Serial2.println("AT+CMQCON=" +   mqttid_sim7020   +  ",3,\" " +  myclient  +  "\",600,0,0");
  delay(2000);
  mqttpub(machineinfo);
    }

    dosome = "none";
  }

  if (dosome == "firstopen")
  {
    runBLE();
    openwifi();
    dosome = "sendmqtt";
  }


  //********************* interrupt check serial every secend***********
  if (do1)
  {
    //Serial.println("do1");
    dosome1();
  } else if (do2)
  {
    //Serial.println("do2");
    dosome2();
  }

}

void dosome2()
{
  String str = "";
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == '\n') {
      if (str.length() > 60)
      {
        machineinfo = str;

        //************* first find machineid to open  **********
        if (machineid.length() < 2)
        {
          dosome = "firstopen";
        } else {
          dosome = "sendmqtt";
        }
        machineid = machineinfo.substring(4, 13);
       // Serial.println(machineid);
      }
    }
    str.concat(c);
  }

  do2 = false;
}

void dosome1()
{
  String str = "";
  while (Serial2.available() > 0)
  {
    char c = Serial2.read();
    if (c == '\n') {
      if (str != "")
      {
        //Serial.print("aha get some from sim7020 ");
        //Serial.println(str);
      }
      str = "";
    }
    str.concat(c);
  }
  if (str != "")
  {
    //Serial.print("aha get some from sim7020 ");
    //Serial.println(str);
  }
  do1 = false;
}


String getmqttid()
{

  String res = "";
  Serial2.println("AT+CMQNEW=\"" +  mqttsvip   +   "\", \"" +  mqttsvport  + "\" ,5000,1024");
  long int time = millis();
  while (time + 1000 > millis())
  {
    while (Serial2.available())
    {
      res +=  Serial2.readString();;
    }
  }
  //Serial.print(res);

  int leng = res.length();
  //Serial.println("aha  find-" + res.substring(leng - 4, leng - 2) + "-"  +  "  -" +   res.substring(leng - 9, leng - 8)   + "-");

  return res.substring(leng - 9, leng - 8);
}


void send_at(String command, int timeout)
{
  Serial2.println(command);
  long int time = millis();
  while (time + timeout > millis())
  {
    while (Serial2.available())
    {
      String c = Serial2.readString();
      //Serial.print(c);
    }
  }
}

void runBLE()
{
  BLEDevice::init(machineid.c_str());
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  c_wifistate = pService->createCharacteristic(
                  wifistate_UUID,
                  BLECharacteristic::PROPERTY_READ |
                  BLECharacteristic::PROPERTY_WRITE
                );
  c_wifistate->setCallbacks(new MyCallbacks());
  c_wifistate->setValue("OFF");

  c_ssid = pService->createCharacteristic(
             ssid_UUID,
             BLECharacteristic::PROPERTY_READ |
             BLECharacteristic::PROPERTY_WRITE
           );
  c_ssid->setValue(ssid.c_str());

  c_pw = pService->createCharacteristic(
           pw_UUID,
           BLECharacteristic::PROPERTY_READ |
           BLECharacteristic::PROPERTY_WRITE
         );
  c_pw->setValue(password.c_str());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();
  //Serial.println("Ble on");
}
