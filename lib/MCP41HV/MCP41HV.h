#pragma once

// http://www.digikey.com/product-detail/en/microchip-technology/MCP41HV31-103E-ST/MCP41HV31-103E-ST-ND/3872205
// http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en561493
// https://www.arduino.cc/en/Tutorial/DigitalPotControl

#include <stdint.h>
#include <SPI.h>

class MCP41HV {
public:
  MCP41HV(int slaveSelectPin) : 
    slaveSelectPin(slaveSelectPin),
    // max 10MHz, MSB shifted first, SPI mode 0,0 (1,1 also supported)
    spiSettings(1000000, MSBFIRST, SPI_MODE0) { }
    //spiSettings(10000000, MSBFIRST, SPI_MODE0) { }
  ~MCP41HV() {
    
  }

  void setup() {
    pinMode(slaveSelectPin, OUTPUT);
    digitalWrite(slaveSelectPin, HIGH);
    // be sure to call SPI.begin() in the Arduino setup()
  }

  uint8_t readWiperValue() {
    Command cmd = {0};
    cmd.address = 0x00;
    cmd.c0 = 1;
    cmd.c1 = 1;
    
    startTransfer();
    SPI.transfer(*((uint8_t *)&cmd));
    uint8_t value = SPI.transfer(0x00);
    endTransfer();

    return value;
  }

  void writeWiperValue(uint8_t newValue) {
    Command cmd = {0};
    cmd.address = 0x00;
    cmd.c0 = 0;
    cmd.c1 = 0;
    
    startTransfer();
    SPI.transfer(*((uint8_t *)&cmd));
    SPI.transfer(newValue);

    endTransfer();
  }
  
  void incrementWiper() {
    Command cmd = {0};
    cmd.address = 0x00;
    cmd.c0 = 1;
    cmd.c1 = 0;
    
    startTransfer();
    SPI.transfer(*((uint8_t *)&cmd));
    endTransfer();
  }
  
  void decrementWiper() {
    Command cmd = {0};
    cmd.address = 0x00;
    cmd.c0 = 0;
    cmd.c1 = 1;
    
    startTransfer();
    SPI.transfer(*((uint8_t *)&cmd));
    endTransfer();
  }
private:
  struct Command {
    unsigned char         : 1;
    unsigned char cmderr  : 1;
    unsigned char c0      : 1;
    unsigned char c1      : 1;
    unsigned char address : 4;
  };
  
  inline void startTransfer() {
    SPI.beginTransaction(spiSettings);
    digitalWrite(slaveSelectPin, LOW);
    delayMicroseconds(1000);
  }
  inline void endTransfer() {
    SPI.endTransaction();
    delayMicroseconds(1000);
    digitalWrite(slaveSelectPin, HIGH);
  }

  const int slaveSelectPin;
  const SPISettings spiSettings;
};