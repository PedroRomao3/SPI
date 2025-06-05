#include "SPI_MSTransfer_T4.h"
SPI_MSTransfer_T4<&SPI> mySPI;

void setup() {
    Serial.begin(9600);
    Serial.print("!!!!! init !!!!!\n");
    mySPI.begin();
}

void loop() {
    static uint32_t t = millis();
    static uint16_t value1 = 0, value2 = 100;
    static bool increasing = true;
    static uint16_t data[1]; // Define a static array to hold the values

    if (millis() - t > 10) {
        Serial.print("millis: ");
        Serial.println(millis());

        // Send values to widgetID 0x0001
        data[0] = value1;
        mySPI.transfer16(data, 1, 0x0002, millis() & 0xFFFF);

        // Send values to widgetID 0x0002
        data[0] = value2;
        mySPI.transfer16(data, 1, 0x0001, millis() & 0xFFFF);

        // Update values
        if (increasing) {
            value1++;
            value2--;
            if (value1 >= 99)
                increasing = false;
        } else {
            value1--;
            value2++;
            if (value1 <= 1)
                increasing = true;
        }

        t = millis();
    }
}
