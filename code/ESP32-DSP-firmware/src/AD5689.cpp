/**
 * @file AD5689.cpp
 * @brief AD5689 interface implementation
 * @version 0.1
 * 
 */

#include "AD5689.h"


AD5689::AD5689(): cs_(DA5689_SYNC) {
    pinMode(cs_, OUTPUT); 
    digitalWrite(cs_, HIGH);

    SPI.begin(DA5689_SCLK, DA5689_MISO, DA5689_MOSI);

    /**
     * @todo Hardware timer for playback speed.
     * 
     */

}


AD5689::~AD5689() { }

void AD5689::setChannelFromQueue(uint8_t channel){
    SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE1));
    digitalWrite(cs_, LOW);

    uint16_t VOut = playback_.popHead(); // Get oldest value from playback

    // Set correct channel.
    if (channel == 1) SPI.transfer((CMD_WRITE_TO_INPUT_REG << 4) | ADDR_DAC_A);
    else SPI.transfer((CMD_WRITE_TO_INPUT_REG << 4) | ADDR_DAC_B);

    SPI.transfer(VOut >> 8);
    SPI.transfer(VOut);
    digitalWrite(cs_, HIGH);
    SPI.endTransaction();

}


void AD5689::setReference(uint8_t mode) {
    SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE1));
    digitalWrite(cs_, LOW);
    SPI.transfer(CMD_REFERENCE_SOURCE << 4);
    SPI.transfer(0);
    SPI.transfer(mode);
    digitalWrite(cs_, HIGH);
    SPI.endTransaction();
}


void AD5689::reset() {
    SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE1));
    digitalWrite(cs_, LOW);
    SPI.transfer(CMD_SOFWARE_RESET << 4);
    SPI.transfer(0);
    SPI.transfer(0);
    digitalWrite(cs_, HIGH);
    SPI.endTransaction();
}


void AD5689::pwrDn(uint8_t modeA, uint8_t modeB){
    SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE1));
    digitalWrite(cs_, LOW);
    SPI.transfer(CMD_POWER_UP_DOWN_DAC << 4);
    SPI.transfer(0);
    SPI.transfer(modeB << 6 | 0xF << 2 | modeA);
    digitalWrite(cs_, HIGH);
    SPI.endTransaction();
}


void AD5689::pushValToQueue(uint16_t val){
    playback_.pushBack(val);
}


std::ostream &operator<<(std::ostream &os, AD5689 &DAC){ 
    os << DAC.playback_ << "\n";
    return os;
}