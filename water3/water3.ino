
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

// gpio13 rxd2  gpio15 txd2

String thes = "GDSCDC07AH00120190815142460HT0000000000000000000000000000-00025.51111END";

int i=1;

int count[9]  = {0,0,0,0,0,0,0,0,0};

int  maxnum = 12;

SoftwareSerial mySerial(13, 15, false, 256);

const char* ssid = "DSCNAP-2G";      // Wi-Fi SSID
const char* password = "DrinkStation88";    // Wi-Fi Password


const char* hostname = "www.windcoffee.club";
int port = 2000;
const char* id = "drankstation1";

WiFiClient net;
PubSubClient client(net);


void messageReceived(String &topic, String &payload) {

 // if (topic == "DrankStation_setwater")
  Serial.println("topic " +  topic + " reciveing message : " + payload);
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


String machineinfo()
{
char by[1];
memset(by, 0, 1);
itoa (i,by,10);
Serial.println(by); 
//machine =  strcat("DSDA17H000",by);   

char str[124];
strcpy(str, "DRINKSTATION");
strcat(str, "DSDA17H000");
strcat(str, by);
strcat(str,"1712181051470290");
strcat(str, "DSDA17H000");
strcat(str, by);
strcat(str, "2017-12-1810:51:47");
strcat(str, "999999000091");
strcat(str, "0012");

int thenum =  count[i-1];
if(thenum  <  maxnum)
{
count[i-1] = thenum +1;
}

if (count[i-1] > 9)
{
  char by[2];
itoa (count[i-1],by,10);
Serial.println(by); 
strcat(str,"00");
strcat(str,by);
strcat(str,"00");
strcat(str,by);
strcat(str,"00");
strcat(str,by);
strcat(str,"00");
strcat(str,by);
}else{
 char by[1];
itoa (count[i-1],by,10);
Serial.println(by);  
strcat(str,"000");
strcat(str,by);
strcat(str,"000");
strcat(str,by);
strcat(str,"000");
strcat(str,by);
strcat(str,"000");
strcat(str,by);
}

//strcat(str,"0000");
//strcat(str,"0000");
//strcat(str,"0000");
//strcat(str,"0000");


thenum =  maxnum - count[i-1];
if (thenum> 9)
{
  char by[2];
itoa (thenum,by,10);
Serial.println(by); 
strcat(str,"00");
strcat(str,by);
strcat(str,"00");
strcat(str,by);
strcat(str,"00");
strcat(str,by);
strcat(str,"00");
strcat(str,by);
}else{
 char by[1];
itoa (thenum,by,10);
Serial.println(by);  
strcat(str,"000");
strcat(str,by);
strcat(str,"000");
strcat(str,by);
strcat(str,"000");
strcat(str,by);
strcat(str,"000");
strcat(str,by);
}

//strcat(str,"0000");
//strcat(str,"0000");
//strcat(str,"0000");
//strcat(str,"0000");

strcat(str,"4.21");
strcat(str,"7.63");

Serial.println(strlen(str));

if ( 9 == i)
{
  i = 1;
}else{
  i = i+1;
}

return str;  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      String info = machineinfo();
      Serial.println(info);

 client.beginPublish("DrankStation", info.length(), false);
 client.print(info);
 client.endPublish();
 delay(2000);
      
      client.subscribe("DrankStation_setwater");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

 mySerial.begin(115200);

 Serial.begin(115200);
 delay(1000);
 
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

 client.setServer(hostname, port);
 client.setCallback(callback);

  Serial.println("connecting to MQTT...");
  while (!client.connect(id)) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("connected!");
  client.subscribe("DrankStation_setwater"); 
}

void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();

    Serial.print("Publish message: ");
    String info = machineinfo();
    Serial.println(info);

 client.beginPublish("DrankStation", info.length(), false);
 client.print(info);
 client.endPublish();
      
 delay(2000);  
}
