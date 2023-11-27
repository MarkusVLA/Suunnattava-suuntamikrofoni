/**
 * @file AD5689.h
 * @brief  AD5689 DAC SPI Interface for audio playback
 * @version 0.1
 * 
 */


#pragma once

#include <SPI.h>
#include "playbackQueue.h"

#define DA5689_SYNC 11 // IO 11
#define DA5689_SCLK 12 // IO 12
#define DA5689_MISO 10 // IO 10
#define DA5689_MOSI 13 // IO 13

#define PLAYBACKSAMPLERATE 44100 // Hz
#define SPI_CLOCK 10e6 // Hz


#define CMD_WRITE_TO_INPUT_REG          0B0001

#define CMD_UPDATE_DAC_WITH_INPUT_REG   0B0010
#define CMD_WRITE_AND_UPDATE_DAC        0B0011
#define CMD_POWER_UP_DOWN_DAC           0B0100
#define CMD_HARDWARE_LDAC_MASK          0B0101
#define CMD_SOFWARE_RESET               0B0110
#define CMD_REFERENCE_SOURCE            0B0111
#define CMD_DAISY_CHAIN_ENABLE          0B1000
#define CMD_READBACK_ENABLE             0B1001
#define ADDR_DAC_A                      0B0001
#define ADDR_DAC_B                      0B1000
#define ADDR_DAC_AB                     0B1001

#define INTERNAL_REF                    0
#define EXTERNAL_REF                    1


class AD5689 {
private:

    playbackQueue playback_;
    uint8_t cs_;


public:

    AD5689();
    ~AD5689();

    /**
     * @brief Set the DAC output channel value.
     * 
     */
    void setChannelFromQueue(uint8_t channel);

    /**
     * @brief Set DAC reference voltage.
     * 
     */
    void setReference(uint8_t mode);

    /**
     * @brief Pop value from playback queue and send to dac.
     * 
     */

    /**
     * @brief Reset DAC
     * 
     */
    void reset();

    /**
     * @brief Power down
     * 
     * @param modeA 
     * @param modeB 
     */
    void pwrDn(uint8_t modeA, uint8_t modeB);

    /**
     * @brief Push value to playback queue.
     * 
     * @param val 
     */
    void pushValToQueue(uint16_t val);

    friend std::ostream &operator<<(std::ostream &os, AD5689 &DAC);

};



