
#include "spi.hpp"

const uint8_t ch0_com = 0B10000000;
const uint8_t ch1_com = 0B1001000;



class MCP3008{

    private:

        uint8_t command[3];
        uint8_t response[3];




    public:

        MCP3008(){

            // Construct command to read channel 0 in single-ended mode
            this->command[0] = 0b00000001;                  // Start bit
            this->command[1] = 0b10000000;                  // Single-ended mode for channel 0
            this->command[2] = 0;                           // Don't care
        }

        uint16_t read_adc(SPIBus &use_bus){

            use_bus.transfer(this->command, this->response, sizeof(this->command));
            // Extract 10-bit ADC result
            uint16_t adc_result = ((this->response[1] & 0b00000011) << 8) | this->response[2];
            return adc_result;
        }
};