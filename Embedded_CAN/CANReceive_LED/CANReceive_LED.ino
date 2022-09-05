#include <CAN.h>
#define DAC1 25
int i;


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
  if (packetSize) {
    while (CAN.available()) 
    {
      i = CAN.read(); 
      dacWrite(DAC1, i);
      Serial.print(i);
    }
      Serial.println();
    }
 }
