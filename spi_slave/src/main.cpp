#include "SPI_MSTransfer_T4.h"
SPI_MSTransfer_T4<&SPI> mySPI;

void setup()
{
    Serial.begin(115200);
    Serial.print("!!!!! init !!!!!\n");
    mySPI.begin();
}

void loop()
{
    static uint32_t t = millis();
    if (millis() - t > 50)
    {
        Serial.print("millis: ");
        Serial.println(millis());
        mySPI.transfer16((const uint16_t[]){0xB00B, 0xF00D, 0xDEED, 0xAAAA, 0xBBBB}, 5, 0x6969, millis() & 0xFFFF);
        t = millis();
    }
}
