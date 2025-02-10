#include <CAN.h>

static int mode_init = 0;
static int rx_node = 0;
static int tx_node = 0;
const int present_node_add = 0x10;
const int g = 7;
const int n =11;
static int key = 0;

int DHA_encode(int key)
{
  return (int) pow(g,key)%n;
}

int DHA_decode(int key)
{
  return (int) pow(g,key)%n;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Receiver");

  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    //while (1);
  }

}

void loop() {
  char mode;

  Serial.println("Please Enter Mode");
  mode = Serial.read();

  if(mode == 'S')
  { 
    if (mode_init == 0)
    {
      Serial.println("Please enter the Address");
      tx_node = Serial.read();
      CAN.beginPacket(tx_node);

      key = analogRead(13);
      if(key>999)
      {
        key = (int) key/10;
      }
      key = DHA_encode(key);
      if(key>99)
      {
        CAN.write((int)key/10);
        CAN.write((int)key%10);
      }
      else{
        CAN.write(key);
      }
      CAN.endPacket();
    }
    
  }

  delay(1000);

}