#include <Arduino.h>
#include <SPI.h>

#define SPI_SCLK_SPEED 20e6

// ADS8688 Pin Definitions
#define ADS8688_CS_PIN 15
#define ADS8688_SCLK_PIN 14
#define ADS8688_MOSI_PIN 13
#define ADS8688_MISO_PIN 12

#define WRITE 1
#define READ 0

#define AUTO_RST 0xA0
#define STAND_BY_MODE 0x82
#define RST_PR 0x85
#define PWR_DN 0x83
#define NO_OP 0x00


#define MAN_CH0 0xC0
#define MAN_CH1 0xC4
#define MAN_CH2 0xC8
#define MAN_CH3 0xCC
#define MAN_CH4 0xD0
#define MAN_CH5 0xD4
#define MAN_CH6 0xD8
#define MAN_CH7 0xDC


#define AUTO_SEQ_EN 0x01
#define CH_PWR_DN 0x02
#define FEATURE_SELECT 0x03

// input ranges
#define CH0_INPUT_RANGE 0x05
#define CH1_INPUT_RANGE 0x06
#define CH2_INPUT_RANGE 0x07
#define CH3_INPUT_RANGE 0x08
#define CH4_INPUT_RANGE 0x09
#define CH5_INPUT_RANGE 0x0A
#define CH6_INPUT_RANGE 0x0B
#define CH7_INPUT_RANGE 0x0C

#define CH0_EN 0x01
#define CH1_EN 0x02
#define CH2_EN 0x04
#define CH3_EN 0x08
#define CH4_EN 0x10
#define CH5_EN 0x20
#define CH6_EN 0x40
#define CH7_EN 0x80

#define VREF 0x06


class ADS8688 {

private:

  float vref;
  uint8_t cs;
  int mode;


public:

  ADS8688(){
    cs = ADS8688_CS_PIN;
    mode = 0x00;
    vref = 3.30; 
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    SPI.begin(14, 12, 13);
    
  }

  void writeRegister(uint8_t reg, uint8_t val) {

    SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));
    digitalWrite(ADS8688_CS_PIN, LOW);

    SPI.transfer((reg << 1) | 0x01);
    SPI.transfer(val);;
    SPI.transfer(0x00);

    digitalWrite(ADS8688_CS_PIN, HIGH);

    SPI.endTransaction();

    }


  // Fast boi
  void fillReadBuffer(uint16_t * buffer, int buffersize){

    SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));


    for (int i = 0; i < buffersize; i++){
      digitalWrite(ADS8688_CS_PIN, LOW);
      buffer[i] = (uint16_t) SPI.transfer32(NO_OP);
      digitalWrite(ADS8688_CS_PIN, HIGH);
    }
    

    SPI.endTransaction();

  }

  uint16_t cmdRegister(uint8_t reg) {

    SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));

    uint32_t command = 0;
    uint32_t result = 0;
    digitalWrite(ADS8688_CS_PIN, LOW);


    command = (command | reg) << 24;
    result = SPI.transfer32(command);


    
    // SPI.transfer16(reg << 8);
    // int16_t result = 0;
    // result = SPI.transfer16(0x0000);
  
    digitalWrite(ADS8688_CS_PIN, HIGH);
    SPI.endTransaction();

    return result;


  }

  void setChannelSPD(uint8_t flag) {
    setChannelSequence(flag);
    setChannelPowerDown((uint8_t)~flag);
    }

  void setChannelSequence(uint8_t flag) {
    writeRegister(AUTO_SEQ_EN,flag);
    }
  
  void setChannelPowerDown(uint8_t flag) {
    writeRegister(CH_PWR_DN,flag);
    }

  uint8_t readRegister(uint8_t reg) {
    SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));
    digitalWrite(ADS8688_CS_PIN, LOW);


    SPI.transfer((reg << 1) | 0x00);
    SPI.transfer(0x00);
    byte result = SPI.transfer(0x00);

    digitalWrite(ADS8688_CS_PIN, HIGH);

    SPI.endTransaction();
    return result;
    }


  uint16_t autoRst() {
    return cmdRegister(AUTO_RST);
    }

  void set_auto_scan_sequence(uint32_t seq){
    writeRegister(AUTO_SEQ_EN, seq);
  }

  void setGlobalRange(uint8_t range) {
    for (uint8_t i=0;i<8;i++) setChannelRange(i,range);
    }

   void setChannelRange(uint8_t ch, uint8_t range) {
      uint8_t reg;
      switch (ch) {

          case 0:  reg = CH0_INPUT_RANGE;break;
          case 1:  reg = CH1_INPUT_RANGE;break;
          case 2:  reg = CH2_INPUT_RANGE;break;
          case 3:  reg = CH3_INPUT_RANGE;break;
          case 4:  reg = CH4_INPUT_RANGE;break;
          case 5:  reg = CH5_INPUT_RANGE;break;
          case 6:  reg = CH6_INPUT_RANGE;break;
          case 7:  reg = CH7_INPUT_RANGE;break;
          default: reg = CH0_INPUT_RANGE;break;
          }

      writeRegister(reg,range);
    }


    uint16_t noOp(){
      return cmdRegister(NO_OP);

    }

    float valToVoltage(uint16_t val) {
    return (float)val * (1.25 * vref) / 65535;
    
    }
};