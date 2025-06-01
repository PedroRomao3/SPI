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
  if ( millis() - t > 50 ) {
    Serial.println(millis());

    mySPI1234.poll_slave();

    t = millis();
  }

}
