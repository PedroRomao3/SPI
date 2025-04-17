#include <SPI.h>

// Define the chip select pin
const int CS_PIN = 52;

void setup() {
    // Set CS pin as output and deselect the slave (active low)
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    // Initialize SPI
    SPI.begin();

    // Configure SPI: 10 MHz clock, MSB first, SPI Mode 0
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

    // Initialize serial communication for debugging
    Serial.begin(9600);
    while (!Serial) {} // Wait for serial port to connect (optional for Teensy)
}

/**
 * Sends a command to the slave and retrieves the response.
 * @param command The command byte to send
 * @return The response byte from the slave
 */
uint8_t sendCommand(uint8_t command) {
    // First transaction: Send the command
    digitalWrite(CS_PIN, LOW);         // Select the slave
    SPI.transfer(command);             // Send command, ignore received byte
    digitalWrite(CS_PIN, HIGH);        // Deselect the slave

    // Optional delay if the slave needs time to process (uncomment if needed)
    // delayMicroseconds(10);

    // Second transaction: Receive the response
    digitalWrite(CS_PIN, LOW);         // Select the slave again
    uint8_t response = SPI.transfer(0x00); // Send dummy byte, get response
    digitalWrite(CS_PIN, HIGH);        // Deselect the slave

    return response;
}

void loop() {
    // Example: Send command 0x01 to request temperature
    uint8_t temp = sendCommand(0x01);
    Serial.print("Temperature: ");
    Serial.println(temp);

    delay(1000); // Wait 1 second

    // Example: Send command 0x02 to request humidity
    uint8_t hum = sendCommand(0x02);
    Serial.print("Humidity: ");
    Serial.println(hum);

    delay(1000); // Wait 1 second
}