#include <Arduino.h>
#include <WiFi.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#include "serialstreambuf.hpp"
#include "AD5689.h"
#include "buffer.h"
#include "playbackQueue.h"
#include "ADS8688.h"
#include "taskmanager.hpp"

// Buffer characteristics
static const int buffer_size = 2048;
static const int numberOfADCChannels = 6;

// ADC controll object.
ADS8688 ADC(numberOfADCChannels);

// DAC controll object.
AD5689 DAC;

// Use double buffering. one for signal reading and second for signal processing.
Buffer adc_read_buffer(numberOfADCChannels, buffer_size); // data is read from adc to this buffer.
Buffer dsp_buffer(numberOfADCChannels, buffer_size);      // Data is processed in this buffer.

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

// template <typename T>
// void pushDataToPlayback(T data){
//   playbackBuffer.pushBack(data);
// }

// template <typename T>
// T popDataFromPlayback(void){
//   return playbackBuffer.popHead();
// }

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

  Serial.begin(250000);
  // Turning off the WiFi
  WiFi.mode(WIFI_OFF);
  bufferFilledSemaphore = xSemaphoreCreateBinary();
  bufferCopiedSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(bufferCopiedSemaphore);
  //Set up DAC
  DAC.reset();
  // Initialize TaskManager and add tasks
  TaskManager taskManager;
  taskManager.addTask(TaskConfig(core_1_task, "Fill ADC read buffer", 10024, NULL, 1, 1), 0); // Task for core 1 with priortity 0 (Highest priority)
  taskManager.startTask();
  
}

void loop() {

  SerialStreambuf espout; // IO stream for serial port
  std::ostream serialOut(&espout);

  float sample_rate_kHz;
  sample_rate_kHz =  benchmark_adc(adc_read_buffer) / 1e3;
  
  while(true){ 

    // if (xSemaphoreTake(bufferFilledSemaphore, portMAX_DELAY) == pdTRUE) {
    //   // Copy data from adc_read_buffer to dsp_buffer
    //   dsp_buffer.copyBuffer(adc_read_buffer);
    //   // Tell core 1 buffer has been copied.
    //   xSemaphoreGive(bufferCopiedSemaphore);

    // }
    // DAC.pushValToQueue(dsp_buffer.getValue(0, 0));

    float base = millis()/1000.0f;
    uint16_t value = 32767.0f + 32766 * sin(base * 2.0f * PI);

    DAC.pushValToQueue(value); // Push 2
    DAC.setChannelFromQueue(0);
 

    // serialOut << "\n\n Sampling " << dsp_buffer.getXDim() << " channels at " << sample_rate_kHz << " kHz/ch." << std::endl;
    // serialOut << dsp_buffer << std::endl;

    serialOut << "Queue: " << DAC << std::endl;

    // DAC.setChannelFromQueue(0);

  }

}
