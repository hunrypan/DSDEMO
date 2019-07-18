#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TIMEOUT 200 

String mqttid_sim7020 = "0";
String mqttsvip = "94.191.14.111";
String mqttsvport = "2000";

//********wifi********************
const char* ssid = "DSCNRT-2.4G";
const char* password = "DrinkStation88";

WiFiClient espClient;
PubSubClient client(espClient);

String  netstate  = "none";

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
netstate = "wifi";
Serial.println("wifi ok");
}
}



void sim7020open()
{
send_at("AT+CMQNEW=?",2000);
delay(2000);
 
//send_at("AT+CMQNEW=\"" +  mqttsvip   +   "\", \"" +  mqttsvport  + "\" ,5000,1024",5000);
//delay(5000);

delay(4000);
mqttid_sim7020 =  getmqttid();
Serial.println("aha mqttid is  " + mqttid_sim7020 );

send_at("AT+CMQCON=" +   mqttid_sim7020   +  ",3,\"myclient\",600,0,0",3000);
delay(3000);

send_at("AT+CMQSUB=" + mqttid_sim7020  + ",\"hello\",0",3000);
delay(3000);
  
}

void setup() {

  Serial.begin(115200);
  delay(500);

  Serial1.begin(115200,SERIAL_8N1, 4, 2); 
  Serial2.begin(115200,SERIAL_8N1, 16, 17);
  
  delay(1000);

 // if (ssid.length > 0)
 { 
  wifiopen(6000);
if(netstate == "none")
{
  Serial.println("to open sim7020");
  sim7020open();
}
 }

 
//send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0,4,\"wind\"",3000);
//delay(3000);
}

void loop() {

  send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0,4,\"1234\"",3000);
  
  delay(6000);
  // put your main code here, to run repeatedly:

}


String getmqttid()
{  
  
 String res = ""; 
Serial2.println("AT+CMQNEW=\"" +  mqttsvip   +   "\", \"" +  mqttsvport  + "\" ,5000,1024");
long int time = millis(); 
while(time + 2000 > millis())
{  
while(Serial2.available())
{
   res+=  Serial2.readString();;
}
}
 Serial.print(res); 

int leng = res.length();
  Serial.println("aha  find-" + res.substring(leng-4,leng-2) + "-"  +  "  -" +   res.substring(leng-9,leng-8)   + "-");
  
 return res.substring(leng-9,leng-8);
}


void send_at(String command, int timeout)
{  
 
Serial2.println(command);
long int time = millis(); 
while(time + timeout > millis())
{  
while(Serial2.available())
{
  String c = Serial2.readString();
  Serial.print(c); 
}
}
}
