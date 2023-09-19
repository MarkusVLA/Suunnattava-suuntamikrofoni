
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "spi.hpp"
#include "MCP3008.hpp"

#include "esp_log.h"

void fill_buffer_from_adc(uint16_t * buffer, int buffersize, MCP3008 &adc, SPIBus &bus){

    // zero out buffer:
    // memset(buffer, 0, buffersize);

    for (int i = 0; i < buffersize; i++){
        buffer[i] = adc.read_adc(bus);
    }
}

extern "C" void app_main (void){

    SPIBus mcp3008_buss(32, 33, 25, 26); // mosi, miso sclk cs
    MCP3008 adc;

    const int buffersize = 20000;

    uint16_t * adc_read_buffer = new uint16_t [buffersize];

    while (true) {
        
        printf("Filling buffer...\n");

        fill_buffer_from_adc(adc_read_buffer, buffersize, adc, mcp3008_buss);
        printf("Done!\n");

        for (int i = 0; i < 16; i++){
            printf("%d, ", adc_read_buffer[i]);  
        }

        printf("\n\n\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    delete [] adc_read_buffer;

}