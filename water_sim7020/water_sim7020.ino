
String s2_command = "";
String mqttid_sim7020 = "0";
String mqttsvip = "94.191.14.111";
String mqttsvport = "2000";
String sim7020_isopen = "off";

void sim7020open()
{
  send_at("AT+CMQNEW=?", 2000);
  delay(2000);

  delay(4000);
  mqttid_sim7020 =  getmqttid();
  Serial.println("aha mqttid is  " + mqttid_sim7020 );

  send_at("AT+CMQCON=" +   mqttid_sim7020   +  ",3,\"myclient\",600,0,0", 3000);
  delay(3000);

  send_at("AT+CMQSUB=" + mqttid_sim7020  + ",\"hello\",0", 3000);
  delay(3000);

  sim7020_isopen = "on"; 
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

  /*
  long int time = millis();
  while (time + timeout > millis())
  {
    while (Serial2.available())
    {
      String c = Serial2.readString();
      Serial.print(c);
    }
  }
  */
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


void watch()
{

if (sim7020_isopen == "off")
{
  sim7020open();
}

  
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
int aha = -1;
aha = com.indexOf("+CMQPUB:") ;
Serial.println((String)aha);  
if (aha != -1)
{
Serial.println(com);
  
  String topic = getValue(com, ',', 1);
  topic = topic.substring(2,topic.length()-1);
  
  String msg =getValue(com, ',',6);
  msg = msg.substring(2,msg.length()-1);
  
  Serial.print("topic is:");
  Serial.println(topic);
  Serial.print("msg is:");
  Serial.println(bytestostring(msg));
}
   //mqttpub("ok");
} 


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
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
//Serial.print("string is "); 
//Serial.println((char*)ahabyte);

char str[sizeof(ahabyte)*2] = "";
array_to_string(ahabyte, sizeof(ahabyte), str);
//Serial.println(str);

 send_at("AT+CMQPUB=" + mqttid_sim7020  + ",\"hello\",1,0,0," + (String)strlen(str)  +  ",\"" + str   + "\"", 3000);
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

void loop() {
  // put your main code here, to run repeatedly:
watch();
//mqttpub("aha ok");
//delay(5000);
  }


  
