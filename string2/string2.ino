String s2_command = "00D72040D58D2040A56520405DF520405DB52040858D2040DD652040DDF52040758D20407D9F20409F9F20409D9D2040919D20409D9B20409D9F20409D9520409B992040939F20409D8F20409F772040597720407D9D20408D7D20409F9F20409F9D20409B9F20409D912040A59D20409DA520409B9D2040BF9F20409D8B2040959B20408B992040938D20408D8D20408D8D20408D9F20409F9F20409DBF2040BF9F20409F9F20409F9F20409F9F20409F75204063772040BF9B20408DA320409FA520409FA320409F5720409D9D2040579B20409D9D20409D9D20409D9D20409D9D20409D9D20409D9D20405975204063772040BFFF2040FFFF2040FFFF2040FFFF2040FFFF2040FFFF2040FFFF2040FFFF2040FFFF2040FFFF2040EBFF2040";


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


 Serial.println(s2_command.length());
// Serial.println(utf8bytestostring(s2_command)); 
}




void loop() {

/*
byte high = buf[1]>>4;
byte low = buf[1]&15;

itoa (high & 0xFF,str1,16);
Serial.print(str1);

itoa (low & 0xFF,str2,16);
Serial.print(str2);
*/


watch3();

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
