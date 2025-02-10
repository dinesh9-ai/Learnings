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
    while (1);
  }

}

void loop() {
  int packetSize = CAN.parsePacket();

  if(packetSize)
  {
    if(mode_init == 0)
    {
      int temp =  (int)CAN.read();
      temp = temp*10 + (int)CAN.read();
      key = DHA_decode(temp);
    }
  }


}
