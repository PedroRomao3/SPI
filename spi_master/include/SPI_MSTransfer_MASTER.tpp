#pragma once
#include <SPI_MSTransfer_MASTER.h>
#include "Arduino.h"
#include "SPI.h"

SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::begin() {
#if defined(__IMXRT1062__)
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_01 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 SDI (MISO) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_02 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 SDO (MOSI) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_03 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 SCK (CLK) */
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_00 = IOMUXC_PAD_DSE(3) | IOMUXC_PAD_SPEED(3) | IOMUXC_PAD_PKE; /* LPSPI4 PCS0 (CS) */

  /* IOMUXC_PAD_DSE - Drive Strength Field
   * 000 output driver disabled
   * 001  R0(150 Ohm @ 3.3V, 260 Ohm@1.8V)
   * 010  R0/2
   * 011  R0/3
   * 100  R0/4
   * 101  R0/5
   * 110  R0/6 -> Default
   * 111  R0/7
   */

  /* IOMUXC_PAD_SPEED - Speed Field
   * 00 50MHz
   * 01 100MHz
   * 10 150MHz -> Default
   * 11 200MHz
   */

  /* IOMUXC_PAD_PKE - Pull / Keep Enable Field
   * 0 Pull/Keeper Disabled
   * 1 Pull/Keeper Enabled -> Default
   */

#endif
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::spi_assert() {
  port->beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE0));
  ::digitalWriteFast(cs_pin, LOW);
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::spi_deassert() const {
  ::digitalWriteFast(cs_pin, HIGH);
  port->endTransaction();
  if ( delayed_transfers ) delayMicroseconds(delayed_transfers);
}


SPI_MSTransfer_MASTER_FUNC uint16_t SPI_MSTransfer_MASTER_OPT::spi_transfer16(const uint16_t data) const {
  if ( delayed_transfers ) delayMicroseconds(delayed_transfers);
  return port->transfer16(data);
}


SPI_MSTransfer_MASTER_FUNC uint16_t SPI_MSTransfer_MASTER_OPT::transfer16(const uint16_t *buffer, const uint16_t length, const uint16_t packetID) const {
  uint16_t buf[5 + length] = { 0xDEAD, slave_ID, static_cast<uint16_t>(length + 1), 0xFAF, packetID };
  for ( int i = 0; i < length; i++ ) buf[i + 5] = buffer[i];
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
  return buf[0];
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::digitalWrite(const uint8_t pin, const bool state) const {
  uint16_t buf[5] = { 0xDEAD, slave_ID, 1, 0x1010, static_cast<uint16_t>((pin << 8) | state) };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
}


SPI_MSTransfer_MASTER_FUNC int SPI_MSTransfer_MASTER_OPT::digitalRead(const uint8_t pin) const {
  uint16_t buf[5] = { 0xDEAD, slave_ID, 1, 0x1011, pin };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
  return buf[2];
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::pinMode(const uint8_t pin, const uint8_t state) const {
  uint16_t buf[5] = { 0xDEAD, slave_ID, 1, 0x1012, static_cast<uint16_t>((pin << 8) | state) };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
}


SPI_MSTransfer_MASTER_FUNC uint16_t SPI_MSTransfer_MASTER_OPT::analogRead(const uint8_t pin) const {
  uint16_t buf[5] = { 0xDEAD, slave_ID, 1, 0x1013, pin };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
  return buf[2];
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::analogReadResolution(const uint8_t bits) const {
  uint16_t buf[5] = { 0xDEAD, slave_ID, 1, 0x1014, bits };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::analogWrite(const uint8_t pin, const uint16_t val) const {
  uint16_t buf[6] = { 0xDEAD, slave_ID, 2, 0x1015, pin, val };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::analogWriteResolution(const uint8_t bits) const {
  uint16_t buf[5] = { 0xDEAD, slave_ID, 1, 0x1016, bits };
  process_data(buf, ((sizeof(buf) >> 1) - 4), buf[3]);
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::process_data(uint16_t *buffer, const uint16_t length, uint16_t command) const {
  uint16_t checksum = 0, csum_passed = 0;
  spi_assert();
  for ( uint16_t i = 0; i < length + 4; i++ ) {
    (void)spi_transfer16(buffer[i]);
    if ( i > 3 ) checksum ^= buffer[i];
  } (void)spi_transfer16(checksum);
  for ( uint16_t i = 0, result = 0; i < 10; i++ ) {
    result = spi_transfer16(0xFFFF);
    if ( result == 0xA5A5 ) { csum_passed = 1; break; }
    if ( result == 0xE0E0 ) break;
    if ( result == 0xFFEA ) {
      checksum = 0xFFEA;
      csum_passed = 0;
      buffer[1] = spi_transfer16(0xFFFF);
      for ( int f = 2; f < buffer[1]; f++ ) buffer[f] = spi_transfer16(0xFFFF);
      for ( uint16_t c = 1; c < buffer[1] - 1; c++ ) checksum ^= buffer[c];
      if ( checksum == buffer[buffer[1] - 1] ) {
        csum_passed = 1;
        break;
      }
    }
  }
  spi_deassert();
  buffer[0] = csum_passed;
}


SPI_MSTransfer_MASTER_FUNC void SPI_MSTransfer_MASTER_OPT::detectSlaves() const {
  uint8_t slave_count = 0;
  spi_assert();
  (void)spi_transfer16(0xBEEF);
  Serial.println("\n  Detected slaves: ");
  for ( uint16_t i = 0, data = 0, start = 0; i < 30; i++ ) {
    data = spi_transfer16(0xFFFF);
    if ( start ) {
      if ( data == 0xFFFF ) break;
      Serial.printf("    Slave %d --> ID: 0x%04X\n", ++slave_count, data);
    }
    else if ( data == 0xBEEF ) start = 1;
  }
  spi_deassert();
  if ( slave_count ) {
    Serial.printf("    Mode: %s\n\n", (slave_count > 1) ? "Daisy-Chained" : "Standalone");
    return;
  }
  Serial.printf("    No slaves detected, check connections.\n\n");
}


SPI_MSTransfer_MASTER_FUNC uint32_t SPI_MSTransfer_MASTER_OPT::events() const {
  uint16_t queues = 0;
  spi_assert();
  (void)spi_transfer16(0xDEAD);
  (void)spi_transfer16(slave_ID);
  (void)spi_transfer16(1);
  (void)spi_transfer16(0xF1A0);
  (void)spi_transfer16(0xF1A0);
  (void)spi_transfer16(0xF1A0); /* can't checksum one word, so resend as checksum */
  for ( uint16_t i = 0, result = 0; i < 10; i++ ) {
    result = spi_transfer16(0xFFFF);
    if ( !queues && (result & 0xFF00) == 0xAD00 ) queues = static_cast<uint8_t>(result);
  }
  if ( queues ) {
    for ( uint16_t i = 0, result = 0; i < 10; i++ ) {
      if ( result == 0xD632 ) break; /* we also break this loop after a dequeue */
      result = spi_transfer16(0xCEB6);
      if ( result == 0xAA55 ) {
        uint16_t buf[SPI_MST_DATA_BUFFER_MAX] = { 0xAA55, 10 }, pos = 1, checksum = 0xAA55;
        for ( uint16_t j = 0; j < buf[1]; j++ ) {
          buf[pos] = spi_transfer16(0xCEB6);
          if ( pos < buf[1] - 1 ) checksum ^= buf[pos];
          pos++;
          if ( pos >= buf[1] ) {
            if ( checksum == buf[buf[1]-1] ) {
              AsyncMST info; info.slaveID = buf[4]; info.packetID = buf[5];
              if ( _master_handler != nullptr ) _master_handler(buf + 6, buf[1] - 7, info);
              for ( uint16_t k = 0, result = 0; k < 10; k++ ) {
                result = spi_transfer16(0xCE0A);
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
