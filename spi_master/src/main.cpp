#include "SPI.h"

#include "SPI_MSTransfer_MASTER.h"
SPI_MSTransfer_MASTER<&SPI, 10, 0x1234> mySPI1234;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(10, OUTPUT);
  digitalWrite(10, 1);
  mySPI1234.begin();
}

void loop() {

  static uint32_t t = millis();
  if ( millis() - t > 100 ) {
    Serial.println(millis());

    uint16_t buf2[1] = { 0xBEEF};
    mySPI1234.transfer16(buf2, 1, 0x1000);

    mySPI1234.pinMode(5, INPUT);
    t = millis();
  }

}
