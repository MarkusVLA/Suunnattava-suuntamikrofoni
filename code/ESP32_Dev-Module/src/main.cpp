#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <SPI.h>


static const int buffer_size = 22000;
static const uint8_t cs = 26;
static const uint8_t miso = 32;
static const uint8_t sclk = 25;
static const uint8_t mosi = 33;


// Use double buffering. one for signal reading and second for signal processing.
uint16_t * adc_read_buffer = new uint16_t [buffer_size]; // Data is read in to this buffer
uint16_t * dsp_buffer = new uint16_t [buffer_size];      // Data is processed in this buffer.


// Buffer has been filled
SemaphoreHandle_t bufferFilledSemaphore = NULL;
// Buffer has been copied
SemaphoreHandle_t bufferCopiedSemaphore = NULL;


int read_ADC(void){

  digitalWrite(cs, LOW);
  uint16_t mask;
  SPI.transfer(0x03); // Single ended mode CH0
  mask = mask | ((SPI.transfer(0x00) << 8) | SPI.transfer(0x00)) >> 1;

  // mask = mask | (SPI.transfer(0x00);   // OR mask with data
  // mask = mask << 8;                    // shift mask 8 bits to the left
  // mask = mask | (SPI.transfer(0x00)    // OR mask with second part of data
  // mask = mask >> 1;                    // shit completed mask 1 bit to the right to get completed 10 bit int in 16 bit integer

  digitalWrite(cs, HIGH);
  
  return mask; 

}

void fill_adc_buffer(u_int16_t * buffer, int buffersize){
  // Clear out buffer
  memset(buffer, 0, buffersize * sizeof(uint16_t));
  // fill buffer
  for (int i = 0; i < buffersize; i++){
    buffer[i] = read_ADC();
  }
}

void core_1_task(void * parameters) {

  while (true){
    // Wait until core 0 signals that the read buffer has been copied and can be filled again safely.
    xSemaphoreTake(bufferCopiedSemaphore, portMAX_DELAY);
    // Fill read buffer
    fill_adc_buffer(adc_read_buffer, buffer_size);
    // Signal to core 0 that read buffer is full.
    xSemaphoreGive(bufferFilledSemaphore);
  }
}


void setup() {  

  Serial.begin(115200);
  pinMode(cs, OUTPUT);
  SPI.begin(sclk, miso, mosi);
  SPISettings test_setting;
  test_setting._clock = 4e6;
  SPI.beginTransaction(test_setting);

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

  // Clear out buffer
  memset(buffer, 0, buffersize * sizeof(uint16_t));

  unsigned long start_time, end_time;

  start_time = micros();
  for (int i = 0; i < buffersize; i++){
    buffer[i] = read_ADC();
  }
  end_time = micros();

  unsigned long elapsedTime = end_time - start_time;
  float sample_rate = (float)buffersize / (elapsedTime / 1000000.0);  // samples per second
  return sample_rate;

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

      Serial.printf("\n\n\nSample rate %2fkHz\nBuffer sample:\n", sample_rate_kHz);
      for (int i = 0; i < 10; i++){

        Serial.printf("%d, ",dsp_buffer[i]); 

      }
    
    }
    
  }

  // Free memory.
  delete [] adc_read_buffer;
  delete [] dsp_buffer;

}
