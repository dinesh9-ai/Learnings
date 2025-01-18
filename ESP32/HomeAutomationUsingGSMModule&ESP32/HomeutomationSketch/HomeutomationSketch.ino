#include <SoftwareSerial.h>

SoftwareSerial mySerial(29, 28); //29PIN -TX , 28PIN - RX


#define FAN1      15
#define LIGHT1    16
#define SWITCH1   17
static int fan_state = 0;
static int light_state = 0;
static int switch_state = 0;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  pinMode(FAN1,OUTPUT);
  pinMode(SWITCH1,OUTPUT);
  pinMode(LIGHT1,OUTPUT);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  mySerial.write(Serial.read());
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  mySerial.write(Serial.read());
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  mySerial.write(Serial.read());
}

void loop()
{
  if(check_message())
  {
    String a = String(mySerial.read()) ;

    // FOR FAN 1
    if (a.substring(0) == "FAN")
    {

      if(a.substring(0) == "STATUS")
      {
        mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
        updateSerial();
        if(fan_state == 0)
        {
          mySerial.println("FAN is OFF");
        }
        else
        {
          mySerial.println("FAN is ON");
        }
        updateSerial();
        mySerial.write(26);
      }
      else if(a.substring(0) == "ON")
      {
        
        if(fan_state == 0)
        {
          fan_state = 1;
          digitalWrite(FAN1,HIGH);
        }
        else
        {
          mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.println("FAN is ALready ON");
          updateSerial();
          mySerial.write(26);
        }

      }
      else if(a.substring(0) == "OFF")
      {

        if(fan_state == 1)
        {
          fan_state = 0;
          digitalWrite(FAN1,LOW);
        }
        else
        {
          mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.println("FAN is ALready OFF");
          updateSerial();
          mySerial.write(26);
        }
      }

    }
    // FOR LIGHT1
    else if (a.substring(0) == "LIGHT")
    {

      if(a.substring(0) == "STATUS")
      {
        mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
        updateSerial();
        if(light_state == 0)
        {
          mySerial.println("LIGHT is OFF");
        }
        else
        {
          mySerial.println("LIGTH is ON");
        }
        updateSerial();
        mySerial.write(26);
      }
      else if(a.substring(0) == "ON")
      {
        
        if(light_state == 0)
        {
          light_state = 1;
          digitalWrite(LIGHT1,HIGH);
        }
        else
        {
          mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.println("LIGHT is Already ON");
          updateSerial();
          mySerial.write(26);
        }

      }
      else if(a.substring(0) == "OFF")
      {

        if(light_state == 1)
        {
          light_state = 0;
          digitalWrite(LIGHT1,LOW);
        }
        else
        {
          mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.println("LIGHT is Already OFF");
          updateSerial();
          mySerial.write(26);
        }
      }

    }
    // FOR SWITCH 1
    else if (a.substring(0) == "SWITCH")
    {

      if(a.substring(0) == "STATUS")
      {
        mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
        updateSerial();
        if(switch_state == 0)
        {
          mySerial.println("SWITCH is OFF");
        }
        else
        {
          mySerial.println("SWITCH is ON");
        }
        updateSerial();
        mySerial.write(26);
      }
      else if(a.substring(0) == "ON")
      {
        
        if(switch_state == 0)
        {
          switch_state = 1;
          digitalWrite(SWITCH1,HIGH);
        }
        else
        {
          mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.println("SWITCH is Already ON");
          updateSerial();
          mySerial.write(26);
        }

      }
      else if(a.substring(0) == "OFF")
      {

        if(switch_state == 1)
        {
          switch_state = 0;
          digitalWrite(FAN1,LOW);
        }
        else
        {
          mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
          updateSerial();
          mySerial.println("SWITCH is ALready OFF");
          updateSerial();
          mySerial.write(26);
        }
      }

    }

  }
}

int check_message(void)
{
  return mySerial.available();
}


void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}