
String s2_command = "";
String mqttid_sim7020 = "0";
String mqttsvip = "94.191.14.111";
String mqttsvport = "2000";

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

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial1.begin(115200, SERIAL_8N1, 4, 2);
  delay(500);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  delay(500);

  sim7020open();

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


void mqttpub(String msg)
{
byte   ahabyte[msg.length()]; 
msg.getBytes(ahabyte,sizeof(ahabyte)+1);
Serial.print("string is "); 
Serial.println((char*)ahabyte);

char str[sizeof(ahabyte)*2] = "";
array_to_string(ahabyte, sizeof(ahabyte), str);
Serial.println(str);

 send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0," + (String)strlen(str)  +  ",\"" + str   + "\"", 3000);
}

void loop() {
  // put your main code here, to run repeatedly:

mqttpub("aha ok");
delay(5000);
  }
