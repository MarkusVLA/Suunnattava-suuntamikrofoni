#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "playbackQueue.h"
#include "ADS8688.h"
#include <SPI.h>

static const int buffer_size = 1024;
static const int sample_rate = 44100; // Hz

static const uint8_t cs = 26;
static const uint8_t miso = 32;
static const uint8_t sclk = 25;
static const uint8_t mosi = 33;

// Use double buffering. one for signal reading and second for signal processing.
uint16_t * adc_read_buffer = new uint16_t [buffer_size]; // Data is read in to this buffer
uint16_t * dsp_buffer = new uint16_t [buffer_size];      // Data is processed in this buffer.
// Playback queue buffer.
playbackQueue playbackBuffer;

// Buffer has been filled
SemaphoreHandle_t bufferFilledSemaphore = NULL;
// Buffer has been copied
SemaphoreHandle_t bufferCopiedSemaphore = NULL;


// ADC object.
ADS8688 ADC;



void fillADCReadBuffer(uint16_t * buffer, int buffersize){
  ADC.fillReadBuffer(buffer, buffer_size);
}


void core_1_task(void * params) {
  while (true){
    // Wait until core 0 signals that the read buffer has been copied and can be filled again safely.
    xSemaphoreTake(bufferCopiedSemaphore, portMAX_DELAY);
    // Fill read buffer
    fillADCReadBuffer(adc_read_buffer, buffer_size);
    // Signal to core 0 that read buffer is full.
    xSemaphoreGive(bufferFilledSemaphore);
  }
}


void setup() {  
  // Initalize ads8688
  ADC.setChannelSPD(0x01); // Channel selection.
  ADC.setGlobalRange(VREF);
  ADC.autoRst();

  Serial.begin(900000);

  bufferFilledSemaphore = xSemaphoreCreateBinary();
  bufferCopiedSemaphore = xSemaphoreCreateBinary();


  xTaskCreatePinnedToCore(
    core_1_task,   
    "Fill ADC read buffer",     
    10000,          // Stack size
    NULL,           // Input
    1,              // priority
    NULL,           // handle
    1);             // core number

}


float benchmark_adc(uint16_t * buffer, int buffersize){
  unsigned long start_time, end_time;
  start_time = micros();
  ADC.fillReadBuffer(buffer, buffer_size);
  end_time = micros();
  unsigned long elapsedTime = end_time - start_time;
  float sample_rate = (float)buffersize / (elapsedTime / 1000000.0);  // samples per second
  return sample_rate;

}



void log_data_to_serial(float data){
  Serial.printf("%f\n",data);
}

void addDataToPlayback(float data){
  playbackBuffer.pushBack(data);
}

float popDataFromPlayback(void){
  return playbackBuffer.popHead();
}


void loop() {

  float sample_rate_kHz;
  sample_rate_kHz =  benchmark_adc(adc_read_buffer, buffer_size) / 1e3;
  xSemaphoreGive(bufferCopiedSemaphore);


  

  while(true){ 

    if (xSemaphoreTake(bufferFilledSemaphore, portMAX_DELAY) == pdTRUE) {

      // Copy data from adc_read_buffer to dsp_buffer
      memcpy(dsp_buffer, adc_read_buffer, buffer_size * sizeof(uint16_t));
        // Tell core 1 buffer has been copied.
      xSemaphoreGive(bufferCopiedSemaphore);
    }

    //  Serial.printf("\n\n\nSample rate %2fkHz\nBuffer sample:\n", sample_rate_kHz);
    // for (int i = 0; i < 3; i++){
    //   Serial.printf("Val: %D, ", dsp_buffer[i]); // convert data to range 0-1;
    // }


    // Serial.printf("\n\n\nSample rate %2fkHz\nBuffer sample:\n", sample_rate_kHz);
    for (int i = 0; i < buffer_size; i++){
      addDataToPlayback((float) dsp_buffer[i] / 65535); // convert data to range 0-1;
    }


    while (!playbackBuffer.isEmpty()) {
      log_data_to_serial(popDataFromPlayback());
    }

  }

  // Free memory.
  delete [] adc_read_buffer;
  delete [] dsp_buffer;

}
