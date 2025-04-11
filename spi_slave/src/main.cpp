/*author: insert link*/
#include <cstdint>
#include "SPISlave_T4.h"
uint32_t spiRx[10];
volatile int spiRxIdx;
volatile int spiRxComplete = 0;


SPISlave_T4<&SPI, SPI_8_BITS> mySPI;

void setup() {
  Serial.begin(115200);	//baudrate does not matter (is USB VCP anyway)
  while ( ! Serial) {}
  Serial.println("START...");
  mySPI.begin();
}

void loop() {
  int i;

  Serial.print("millis: "); Serial.println(millis());
  if (spiRxComplete) {
    Serial.println(spiRxIdx);
    for (i = 0; i < spiRxIdx; i++) {
      Serial.print(spiRx[i], HEX); Serial.print(" ");
    }
    Serial.println();
    spiRxComplete = 0;
    spiRxIdx = 0;
  }
  delay(1000);
}

