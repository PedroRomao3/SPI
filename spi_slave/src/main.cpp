#include "SPI_MSTransfer_T4.h"
SPI_MSTransfer_T4<&SPI, 0x1234> mySPI;

void myCB(uint16_t* buffer, uint16_t length, AsyncMST info)
{
    for (int i = 0; i < length; i++)
    {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.print(" --> Length: ");
    Serial.print(length);
    Serial.print(" --> PacketID: ");
    Serial.println(info.packetID, HEX);
}

void setup()
{
    Serial.begin(115200);
    Serial.print("!!!!! init !!!!!\n");
    mySPI.begin();
    // mySPI.onTransfer(myCB);
}

void loop()
{
    // mySPI.events();
    static uint32_t t = millis();
    if (millis() - t > 100)
    {
        Serial.print("millis: ");
        Serial.println(millis());
        mySPI.transfer16((const uint16_t[]){0xB00B, 0xF00D, 0xDEED}, 3, millis() & 0xFFFF);
        t = millis();
    }
}
