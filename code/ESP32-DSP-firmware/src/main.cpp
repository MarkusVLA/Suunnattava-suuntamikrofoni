#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "buffer.h"
#include "playbackQueue.h"
#include "ADS8688.h"
#include <SPI.h>
#include <cstdint>

// Buffer characteristics
static const int buffer_size = 100;
static const int numberOfADCChannels = 6;

// ADC controll object.
ADS8688 ADC(numberOfADCChannels);
// Use double buffering. one for signal reading and second for signal processing.
Buffer adc_read_buffer(numberOfADCChannels, buffer_size); // data is read from adc to this buffer.
Buffer dsp_buffer(numberOfADCChannels, buffer_size);      // Data is processed in this buffer.

// Playback queue buffer.
playbackQueue playbackBuffer;

// process semaphores
SemaphoreHandle_t bufferFilledSemaphore = NULL; // Read buffer has been filled
SemaphoreHandle_t bufferCopiedSemaphore = NULL; // Read buffer has been copied

void fillADCReadBuffer(Buffer &buffer){
  ADC.fillReadBuffer(buffer);
}

float benchmark_adc(Buffer &buffer){
  // Measure ADC sample rate.
  unsigned long start_time, end_time;
  start_time = micros();
  ADC.fillReadBuffer(buffer);
  end_time = micros();
  unsigned long elapsedTime = end_time - start_time;
  float sample_rate = (float) buffer.getYDim() / (elapsedTime / 1000000.0);  // samples per second
  return sample_rate;
}

void log_data_to_serial(float data){
  Serial.printf("%f\n",data);
}

void pushDataToPlayback(float data){
  playbackBuffer.pushBack(data);
}

float popDataFromPlayback(void){
  return playbackBuffer.popHead();
}

void core_1_task(void * params) {
  while (true){
    // Wait until core 0 signals that the read buffer has been copied and can be filled again safely.
    xSemaphoreTake(bufferCopiedSemaphore, portMAX_DELAY);
    // Fill read buffer
    fillADCReadBuffer(adc_read_buffer);
    // Signal to core 0 that read buffer is full.
    xSemaphoreGive(bufferFilledSemaphore);
  }
}

void setup() {

  // Initalize ads8688
  ADC.setChannelSPD(0B00111111); // Channel selection (0B00111111: channels 0 to 5 are active )
  ADC.setGlobalRange(VREF);      // Set adc voltage refernce flag
  ADC.autoRst();                 // Set ADC to auto reset mode

  Serial.begin(115200);

  bufferFilledSemaphore = xSemaphoreCreateBinary();
  bufferCopiedSemaphore = xSemaphoreCreateBinary();

  xSemaphoreGive(bufferCopiedSemaphore);

  xTaskCreatePinnedToCore(
    core_1_task,   
    "Fill ADC read buffer",     
    10000,          // Stack size
    NULL,           // Input
    1,              // priority
    NULL,           // handle
    1);             // core number

    Serial.printf("Setup complete.");

}

void loop() {

  float sample_rate_kHz;
  sample_rate_kHz =  benchmark_adc(adc_read_buffer) / 1e3;
  
  while(true){ 

    if (xSemaphoreTake(bufferFilledSemaphore, portMAX_DELAY) == pdTRUE) {
      // Copy data from adc_read_buffer to dsp_buffer
      dsp_buffer.copyBuffer(adc_read_buffer);
      // Tell core 1 buffer has been copied.
      xSemaphoreGive(bufferCopiedSemaphore);
    }

    Serial.printf("\n\nSampling %d channels at %fkHz/ch.\n", dsp_buffer.getXDim(), sample_rate_kHz);
    for (int i = 0; i < dsp_buffer.getXDim(); i++){
      Serial.printf("%d, ", dsp_buffer.getValue(i,0));  
    }
    


    delay(500);



  }

}
