#include "ADS8688.h"



// Static member variable definitions
volatile bool ADS8688::read_ADC_flag = false;
hw_timer_t * ADS8688::adc_timer = NULL;

// Static member function implementation
void IRAM_ATTR ADS8688::ADC_timer() {
    read_ADC_flag = true;
}

ADS8688::ADS8688() {

  cs = ADS8688_CS_PIN;
  mode = 0x00;
  vref = 3.30; 
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin(14, 12, 13);

  adc_timer = timerBegin(0, 80, true);  // prescaler ESP32 clock
  timerAttachInterrupt(adc_timer, &ADS8688::ADC_timer, true); // Attach timer to read_adc
  timerAlarmWrite(adc_timer, 1000000 / ADC_SAMPLE_RATE , true);  // Set the timer to trigger at sample rate
  timerAlarmEnable(adc_timer);
}




void ADS8688::writeRegister(uint8_t reg, uint8_t val) {

  SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));
  digitalWrite(ADS8688_CS_PIN, LOW);

  SPI.transfer((reg << 1) | 0x01);
  SPI.transfer(val);;
  SPI.transfer(0x00);

  digitalWrite(ADS8688_CS_PIN, HIGH);

  SPI.endTransaction();

}



  // Fast boi
void ADS8688::fillReadBuffer(uint16_t * buffer, int buffersize){

  SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));

  for (int i = 0; i < buffersize; i++){
    
    // Wait for correct timing
    while(!read_ADC_flag){
        ;;
      }
       
      digitalWrite(ADS8688_CS_PIN, LOW);
      buffer[i] = (uint16_t) SPI.transfer32(NO_OP);
      read_ADC_flag = false;
      digitalWrite(ADS8688_CS_PIN, HIGH);
    }
  
    SPI.endTransaction();

  }
  

uint16_t ADS8688::cmdRegister(uint8_t reg) {

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

void ADS8688::setChannelSPD(uint8_t flag) {
  setChannelSequence(flag);
  setChannelPowerDown((uint8_t)~flag);
  }

void ADS8688::setChannelSequence(uint8_t flag) {
  writeRegister(AUTO_SEQ_EN,flag);
  }

void ADS8688::setChannelPowerDown(uint8_t flag) {
  writeRegister(CH_PWR_DN,flag);
  }

uint8_t ADS8688::readRegister(uint8_t reg) {
  SPI.beginTransaction(SPISettings(SPI_SCLK_SPEED, MSBFIRST, SPI_MODE1));
  digitalWrite(ADS8688_CS_PIN, LOW);


  SPI.transfer((reg << 1) | 0x00);
  SPI.transfer(0x00);
  byte result = SPI.transfer(0x00);

  digitalWrite(ADS8688_CS_PIN, HIGH);

  SPI.endTransaction();
  return result;
  }


uint16_t ADS8688::autoRst() {
  return cmdRegister(AUTO_RST);
  }

void ADS8688::set_auto_scan_sequence(uint32_t seq){
  writeRegister(AUTO_SEQ_EN, seq);
}

void ADS8688::setGlobalRange(uint8_t range) {
  for (uint8_t i=0;i<8;i++) setChannelRange(i,range);
  }

  void ADS8688::setChannelRange(uint8_t ch, uint8_t range) {
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


uint16_t ADS8688::noOp(){
  return cmdRegister(NO_OP);

}

float ADS8688::valToVoltage(uint16_t val) {
  return (float)val * (1.25 * vref) / 65535;
}


