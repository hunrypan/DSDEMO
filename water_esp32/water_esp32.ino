#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TIMEOUT 200
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

String mqttid_sim7020 = "0";
String mqttsvip = "94.191.14.111";
String mqttsvport = "2000";

//********wifi********************
const char* ssid = "DSCNRT-2.4G";
const char* password = "DrinkStation88";

const char* mqttsv = "94.191.14.111";
uint16_t mqttport = 2000;
WiFiClient espClient;
PubSubClient client(espClient);


void wifiopen(int timeout)
{
  WiFi.begin(ssid, password);
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



void sim7020open()
{
  send_at("AT+CMQNEW=?", 2000);
  delay(2000);

  //send_at("AT+CMQNEW=\"" +  mqttsvip   +   "\", \"" +  mqttsvport  + "\" ,5000,1024",5000);
  //delay(5000);

  delay(4000);
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

  // if (ssid.length > 0)
  {
    wifiopen(6000);
    if (WiFi.status() == WL_CONNECTED)
    {
      client.setServer(mqttsv, mqttport);
      client.setCallback(callback);
    } else {
      Serial.println("to open sim7020");
      sim7020open();
    }
  }


  //send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0,4,\"wind\"",3000);
  //delay(3000);
}

void loop() {

  if (WiFi.status() == WL_CONNECTED)
  {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    client.publish("hello", "hello from wifi");
  } else {
    send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0,4,\"1234\"", 3000);
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
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("DSC00000001");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
