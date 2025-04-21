#pragma once

#include "Arduino.h"
#include <functional>
#include <SPI.h>

#define SPI_MST_QUEUE_SLOTS 8
#define SPI_MST_DATA_BUFFER_MAX 250

struct AsyncMST {
  uint16_t packetID = 0;
  uint16_t slaveID = 0;
};

using master_handler_ptr = void(*)(uint16_t* buffer, uint16_t length, AsyncMST info);
using detectPtr = std::function<void(AsyncMST info)>;

#define SPI_MSTransfer_MASTER_CLASS template<SPIClass* port, uint8_t cs_pin, uint32_t slave_ID, uint32_t spi_speed = 2000000>
#define SPI_MSTransfer_MASTER_FUNC template<SPIClass* port, uint8_t cs_pin, uint32_t slave_ID, uint32_t spi_speed>
#define SPI_MSTransfer_MASTER_OPT SPI_MSTransfer_MASTER<port, cs_pin, slave_ID, spi_speed>

class SPI_MSTransfer_MASTER_Base {};

SPI_MSTransfer_MASTER_CLASS class SPI_MSTransfer_MASTER : public SPI_MSTransfer_MASTER_Base {
  public:
    SPI_MSTransfer_MASTER() = default;
    static void begin();
    void onTransfer(const master_handler_ptr handler) { _master_handler = handler; }
    uint16_t transfer16(const uint16_t *buffer, uint16_t length, uint16_t packetID) const;
    void detectSlaves() const;
    uint32_t events() const;
    void pinMode(uint8_t pin, uint8_t state) const;
    void digitalWrite(uint8_t pin, bool state) const;
    void digitalWriteFast(const uint8_t pin, const bool state) const { return digitalWrite(pin, state); }
    int digitalRead(uint8_t pin) const;
    int digitalReadFast(const uint8_t pin) const { return digitalRead(pin); }
    void delayTransfers(const uint16_t uS) { delayed_transfers = uS; }
    uint16_t analogRead(uint8_t pin) const;
    void analogReadResolution(uint8_t bits) const;
    void analogWriteResolution(uint8_t bits) const;
    void analogWrite(uint8_t pin, uint16_t val) const;

  private:
    void process_data(uint16_t *buffer, uint16_t length, uint16_t command) const;
    uint16_t spi_transfer16(uint16_t data) const;

    static void spi_assert();
    void spi_deassert() const;
    int _portnum = 0;
    uint32_t nvic_irq = 0;
    master_handler_ptr _master_handler{};
    volatile uint16_t delayed_transfers = 0;
};

#include "SPI_MSTransfer_MASTER.tpp"
