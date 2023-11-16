/**
 * @file ADS8688.h
 * @brief ADS8688 ADC SPI interface
 * @version 0.1
 * 
 * 
 */

#ifndef ADS8688_H
#define ADS8688_H



#include <Arduino.h>
#include <SPI.h>
#include "esp_timer.h"
#include "buffer.h"

#define SPI_SCLK_SPEED 20e6 
#define ADC_SAMPLE_RATE 44.1e3 // Set sample rate here

// ADS8688 Pin Definitions for esp 
#define ADS8688_CS_PIN 4
#define ADS8688_SCLK_PIN 5
#define ADS8688_MOSI_PIN 2
#define ADS8688_MISO_PIN 3

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
    int nChannels;

    static volatile bool read_ADC_flag;
    static hw_timer_t * adc_timer;

public:
    ADS8688(int nChannels);

    static void IRAM_ATTR ADC_timer();

    void writeRegister(uint8_t reg, uint8_t val);
    void fillReadBuffer(Buffer &buffer);
    uint16_t cmdRegister(uint8_t reg);
    void setChannelSPD(uint8_t flag);
    void setChannelSequence(uint8_t flag);
    void setChannelPowerDown(uint8_t flag);
    uint8_t readRegister(uint8_t reg);
    uint16_t autoRst();
    void set_auto_scan_sequence(uint32_t seq);
    void setGlobalRange(uint8_t range);
    void setChannelRange(uint8_t ch, uint8_t range);
    uint16_t noOp();
    float valToVoltage(uint16_t val);
};


#endif // ADS8688_H