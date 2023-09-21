#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include <SPI.h>



static const int buffer_size = 22000;
static const int sample_rate = 44100; // Hz

static const uint8_t cs = 26;
static const uint8_t miso = 32;
static const uint8_t sclk = 25;
static const uint8_t mosi = 33;

static const uint16_t SERIAL_SYNC_WORD = 0xFFFF; 


// adc timer to get right sample rate.
hw_timer_t * adc_timer = NULL;
volatile bool read_ADC_flag = false;


// Use double buffering. one for signal reading and second for signal processing.
uint16_t * adc_read_buffer = new uint16_t [buffer_size]; // Data is read in to this buffer
uint16_t * dsp_buffer = new uint16_t [buffer_size];      // Data is processed in this buffer.


// Buffer has been filled
SemaphoreHandle_t bufferFilledSemaphore = NULL;
// Buffer has been copied
SemaphoreHandle_t bufferCopiedSemaphore = NULL;



void IRAM_ATTR ADC_timer() {
    read_ADC_flag = true;
}

uint16_t read_ADC(void){
  uint16_t mask;

  digitalWrite(cs, LOW);
  SPI.transfer(0x03); // Single ended mode CH0
  mask = mask | ((SPI.transfer(0x00) << 8) | SPI.transfer(0x00)) >> 1;

  // mask = mask | (SPI.transfer(0x00);   // OR mask with data
  // mask = mask << 8;                    // shift mask 8 bits to the left
  // mask = mask | (SPI.transfer(0x00)    // OR mask with second part of data
  // mask = mask >> 1;                    // shit completed mask 1 bit to the right to get completed 10 bit int in 16 bit integer
  digitalWrite(cs, HIGH);

  while (!read_ADC_flag){

    delayMicroseconds(1);

  }
  read_ADC_flag = false;
  return mask; 

}

void fill_adc_buffer_at_sample_rate(u_int16_t * buffer, int buffersize){
  // Clear out buffer
  memset(buffer, 0, buffersize * sizeof(uint16_t));
  // fill buffer
  for (int i = 0; i < buffersize; i++){

      buffer[i] = read_ADC();

  }
}


void core_1_task(void * params) {

  while (true){
    // Wait until core 0 signals that the read buffer has been copied and can be filled again safely.
    xSemaphoreTake(bufferCopiedSemaphore, portMAX_DELAY);
    // Fill read buffer
    fill_adc_buffer_at_sample_rate(adc_read_buffer, buffer_size);
    // Signal to core 0 that read buffer is full.
    xSemaphoreGive(bufferFilledSemaphore);
  }

}


void setup() {  

  Serial.begin(115200);
  pinMode(cs, OUTPUT);
  SPI.begin(sclk, miso, mosi);
  SPISettings test_setting;
  test_setting._clock = 1e6;
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

    
  // timer is needed to get consitent sample rate.
  adc_timer = timerBegin(0, 80, true);  // prescaler ESP32 clock
  timerAttachInterrupt(adc_timer, &ADC_timer, true); // Attach timer to read_adc
  timerAlarmWrite(adc_timer, 1000000 / sample_rate , true);  // Set the timer to trigger at sample rate
  timerAlarmEnable(adc_timer);

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

void moving_avarage_filter(uint16_t * dsp_buffer, int buffer_length, int window_size = 3){

  if (window_size <= 0 || window_size > buffer_length) return;
  int window_sum = 0;

  for (int i = 0; i < buffer_length; i++){
    if (buffer_length - i >= window_size){
      for (int j = 0; j < window_size; j++){
        window_sum += dsp_buffer[i + j];
      }
      dsp_buffer[i] = window_sum / window_size;
      window_sum = 0;
    }
  }
}


void log_data_to_serial(uint16_t data){

  //Serial.write((byte*)&SERIAL_SYNC_WORD, sizeof(SERIAL_SYNC_WORD));  // Send sync word
  //Serial.write((byte*)&data, sizeof(data));
  //Serial.write(10);  // Send data as binary
  Serial.printf("%d\n",data);

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

      moving_avarage_filter(dsp_buffer, buffer_size, 10);

      // Serial.printf("\n\n\nSample rate %2fkHz\nBuffer sample:\n", sample_rate_kHz);
      for (int i = 0; i < buffer_size; i++){

        //Serial.printf("\n\n%d,", dsp_buffer[i]);
        log_data_to_serial(dsp_buffer[i]);

      }
    }
  }

  // Free memory.
  delete [] adc_read_buffer;
  delete [] dsp_buffer;

}
