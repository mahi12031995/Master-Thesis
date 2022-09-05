
#include <CAN.h>
int i ;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("CAN Sender");
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  CAN.beginPacket(0x12);
  CAN.write(i);
  CAN.endPacket();
  delay(100);
  i++;
}
