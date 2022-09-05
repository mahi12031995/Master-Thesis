#include <CAN.h>

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
    if (CAN.packetRtr()) 
    {
      Serial.print("RTR ");
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
      Serial.print("Sending packet ... ");
      CAN.beginPacket(0x10);
      CAN.write('a');
      CAN.write('b');
      CAN.write('c');
      CAN.endPacket();
    } 
    else 
    {
      while (CAN.available()) 
      {
        Serial.print((char)CAN.read()); 
      }
    }
      Serial.println();
    }
 }
