#include <SPI_MSTransfer_MASTER.h>
#include "Arduino.h"
#include "SPI.h"


SPI_MSTransfer_MASTER_FUNC SPI_MSTransfer_MASTER_OPT::SPI_MSTransfer_MASTER() {
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::begin() {
#if defined(__IMXRT1062__)
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_01 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 SDI (MISO) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_02 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 SDO (MOSI) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_03 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 SCK (CLK) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_00 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 PCS0 (CS) */
#endif
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::spi_assert() {
  port->beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE0));
  ::digitalWriteFast(cs_pin, LOW);
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::spi_deassert() {
  ::digitalWriteFast(cs_pin, HIGH);
  port->endTransaction();
}


SPI_MSTransfer_MASTER_FUNC uint16_t SPI_MSTransfer_MASTER_OPT::spi_transfer16(uint16_t data) {
  return port->transfer16(data);
}

SPI_MSTransfer_MASTER_FUNC uint32_t SPI_MSTransfer_MASTER_OPT::poll_slave() {
  uint16_t r;
  uint16_t queues = 0;
  spi_assert();
  Serial.print("\nSTART: ");
  r = spi_transfer16(0xFEED); // Send feed command
  Serial.printf("  %04X ", r);
  Serial.println();
  Serial.print("QUEUE: ");
  for ( uint16_t i = 0, result = 0; i < 10; i++ ) { // Wait for queue status
    result = spi_transfer16(0xFFFF);
    Serial.printf("  %04X ", result);
    if ( !queues && (result & 0xFF00) == 0x6900 ) {
      queues = (uint8_t)result;
      break;
    }
  }
  if ( queues ) { // If there are queues, process them
    for ( uint16_t i = 0, result = 0; i < 10; i++ ) {
      Serial.println();
      if ( result == 0xD632 ) break; // if done break loop
      Serial.print("POLLING DATA: ");
      result = spi_transfer16(0xF00D);
      Serial.printf("  %04X ", result);
      if ( result == 0xDA7A ) { // Marks the start of a data packet
        // Packet Structure:
        // 0: Header (0xDA7A)
        // 1: Length (number of data words)
        // 2: Widget ID
        // 3: Packet ID
        // 4: Data words (variable length)
        uint16_t buf[SPI_MST_DATA_BUFFER_MAX] = {};
        uint16_t pos = 0;
        uint16_t checksum = 0xDA7A;

        buf[pos] = 0xDA7A;
        pos++;

        Serial.println();
        Serial.print("DATA: ");
        buf[pos] = spi_transfer16(0xF00D); // Data Length + header, length, widgetid, packetid, checksum
        checksum ^= buf[pos];
        Serial.printf("  %04X ", buf[pos]);
        pos++;

        for ( uint16_t i = 0; i < buf[1]; i++ ) {
          buf[pos] = spi_transfer16(0xF00D);
          Serial.printf("  %04X ", buf[pos]);
          if ( pos < buf[1] - 1 ) checksum ^= buf[pos];
          pos++;
          if ( pos >= buf[1] ) {
            if ( checksum == buf[buf[1]-1] ) {
              AsyncMST info; info.widgetID = buf[2]; info.packetID = buf[3];
              // if ( _master_handler != nullptr ) _master_handler(buf + 6, buf[1] - 7, info);
              for ( uint16_t i = 0, result = 0; i < 10; i++ ) {
                result = spi_transfer16(0xCE0A);
                Serial.printf("  %04X ", result);
                if ( result == 0xD632 ) {
                  queues--;
                  break;
                }
              }
              break;
            }
          }
        }
      }
    }
  }
  spi_deassert();
  return queues;
}
