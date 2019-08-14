String eggs1 = "DRINKSTATION DSDA19A0001 171121003425 0180 DSDA19A00012017-11-21 00:34:25 9999990001  0000 0000END 05.6-0.0 T11T2111111111111SEND ";
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

String  gethexstr (String msg)
{
byte   ahabyte[msg.length()]; 
msg.getBytes(ahabyte,sizeof(ahabyte)+1);
char str1[sizeof(ahabyte)*2] ;
array_to_string(ahabyte, sizeof(ahabyte), str1);
return str1;
}

String  hexfrombyte(byte   ahabyte[])
{
char str1[sizeof(ahabyte)*2] ;
array_to_string(ahabyte, sizeof(ahabyte), str1);
return str1;
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


String utf8bytestostring(String hex)
{
  String thes = "";
  unsigned char val[hex.length()/4];
 for (size_t count = 0; count < sizeof val/sizeof *val; count++) {
       sscanf(hex.c_str() + 4*count, "%4hhx", &val[count]);
      char  char1 = char( hex_to_ascii(hex[4*count+2], hex[4*count + 3])); 
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



void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  delay(500);

  //Serial2.begin(115200);
   
   Serial2.begin(115200, SERIAL_8N1, 16, 17);
  delay(500);

// Serial.println(utf8bytestostring(s2_command)); 
}




void loop() {


while (Serial2.available())
{
    char c= Serial2.read();
   Serial.print(c);
   }
   

//watch3();

}

void watch2()
{
if (Serial2.available())
{
  byte buf[2];
  int len = Serial2.readBytes(buf,2);
   if(len>0)
   {
   Serial2.print ("find");
   }else{
    Serial2.println("");
   }
  }
}

void watch3()
{  
if (Serial2.available())
{
    byte buf[1];
    int len = Serial2.readBytes(buf,1);
   if(len>0)
   {
char str1[sizeof(buf)*2] ;
array_to_string(buf, sizeof(buf), str1);
   Serial.print(str1);
   }
}
}

void s2_Parse(String com)
{
 Serial.print("str is: ");
 Serial.println(com);
}
