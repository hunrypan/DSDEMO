#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TIMEOUT 200
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define wifistate_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ssid_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a1"
#define pw_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a2"


String s2_command;

String mqttid_sim7020 = "0";
String mqttsvip = "94.191.14.111";
String mqttsvport = "2000";

//********wifi********************

String ssid =  "DSCNRT-2.4G";
String password = "DrinkStation88";

const char* mqttsv = "94.191.14.111";
uint16_t mqttport = 2000;
WiFiClient espClient;
PubSubClient client(espClient);

 BLECharacteristic *c_ssid = NULL;
 BLECharacteristic *c_pw = NULL;
 BLECharacteristic * c_wifistate = NULL;


 void wifiopen(int timeout)
{
  WiFi.begin(ssid.c_str(), password.c_str());
  long int time = millis();

  while (WiFi.status() != WL_CONNECTED && (time + timeout > millis())) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("wifi ok");
  }
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

std::string newssid = c_ssid->getValue();
std::string newpw = c_pw->getValue();
      
    pCharacteristic->notify();

        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
         Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
       
        ssid = newssid.c_str();
        password = newpw.c_str();

         Serial.println(newssid.c_str());
        Serial.println(newpw.c_str());

    wifiopen(6000);
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("ok wifi connect");
      Serial.println("ok wifi connect");
      client.setServer(mqttsv, mqttport);
      client.setCallback(callback);
      pCharacteristic->setValue("ON");
      pCharacteristic->notify();
    } else {
       pCharacteristic->setValue("OFF");
       pCharacteristic->notify();
      Serial.println("to open sim7020");
    }
      
    }
};

void mqttpub(String msg)
{
byte   ahabyte[msg.length()]; 
msg.getBytes(ahabyte,sizeof(ahabyte)+1);
//Serial.print("string is "); 
//Serial.println((char*)ahabyte);

char str[sizeof(ahabyte)*2] = "";
array_to_string(ahabyte, sizeof(ahabyte), str);
//Serial.println(str);

 send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0," + (String)strlen(str)  +  ",\"" + str   + "\"", 3000);
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

String bytestostring(String hex)
{
  String thes = "";
  unsigned char val[hex.length()/2];
 for (size_t count = 0; count < sizeof val/sizeof *val; count++) {
        sscanf(hex.c_str() + 2*count, "%2hhx", &val[count]);
      char  char1 = char( hex_to_ascii(hex[2*count], hex[2*count + 1])); 
      thes +=  char1;
    }
    return thes;
}

int hex_to_int(char c){
        int first = c / 16 - 3;
        int second = c % 16;
        int result = first*10 + second;
        if(result > 9) result--;
        return result;
}

int hex_to_ascii(char c, char d){
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}



void sim7020open()
{
  send_at("AT+CMQNEW=?", 2000);
  delay(3000);

  mqttid_sim7020 =  getmqttid();
  Serial.println("aha mqttid is  " + mqttid_sim7020 );

  send_at("AT+CMQCON=" +   mqttid_sim7020   +  ",3,\"myclient\",600,0,0", 3000);
  delay(3000);

  send_at("AT+CMQSUB=" + mqttid_sim7020  + ",\"hello\",0", 3000);
  delay(3000);

}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {

  Serial.begin(115200);
  delay(500);

  Serial1.begin(115200, SERIAL_8N1, 4, 2);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  delay(1000);

  runBLE();
  delay(2000);

  if (ssid.length() > 0)
  {
    wifiopen(6000);
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("ok wifi connect");
      client.setServer(mqttsv, mqttport);
      client.setCallback(callback);
    } else {
      Serial.println("to open sim7020");
      sim7020open();
    }
  }
}

void loop() {

  if (WiFi.status() == WL_CONNECTED)
  {     
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    client.publish("hello", "hello from  wifi");
  } else {
     
    mqttpub("wind");
  }
  //

  delay(6000);
  // put your main code here, to run repeatedly:

}


String getmqttid()
{

  String res = "";
  Serial2.println("AT+CMQNEW=\"" +  mqttsvip   +   "\", \"" +  mqttsvport  + "\" ,5000,1024");
  long int time = millis();
  while (time + 2000 > millis())
  {
    while (Serial2.available())
    {
      res +=  Serial2.readString();;
    }
  }
  Serial.print(res);

  int leng = res.length();
  Serial.println("aha  find-" + res.substring(leng - 4, leng - 2) + "-"  +  "  -" +   res.substring(leng - 9, leng - 8)   + "-");

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
      Serial.print(c);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "dsc001";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("hello", "hello world");
      // ... and resubscribe
      client.subscribe("hello");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void runBLE()
{
  BLEDevice::init("DSC");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  c_wifistate = pService->createCharacteristic(
                                         wifistate_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  c_wifistate->setCallbacks(new MyCallbacks());
  if(WiFi.status() == WL_CONNECTED)
  {
  c_wifistate->setValue("ON");  
  }else{
  c_wifistate->setValue("OFF");  
  }

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
  //BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  //pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}


void watch(int timeout)
{
if (Serial2.available())
{
  char c = Serial2.read();
  if (c =='\n')
  {
    s2_Parse(s2_command);
    s2_command = "";
  }else{
    s2_command += c; 
  }
}
} 

void s2_Parse(String com)
{
     Serial.print("Serial2 find  ");
     Serial.println(com);
} 
