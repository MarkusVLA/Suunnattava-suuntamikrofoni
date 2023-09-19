#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <cstring>

 

class SPIBus {

    private: 
        
        // Buss parameters
        spi_device_handle_t handle;
        spi_bus_config_t buss_config;
        spi_device_interface_config_t device_config;
        spi_transaction_t transaction;

    public:

        SPIBus(){};

        SPIBus(int MISO, int MOSI, int SCLK, int CS){
            // Set up Buss config
            memset(&this->buss_config, 0, sizeof(this->buss_config));
            this->buss_config.mosi_io_num = MOSI;
            this->buss_config.miso_io_num = MISO;
            this->buss_config.sclk_io_num = SCLK;
            this->buss_config.quadwp_io_num = -1; 
            this->buss_config.quadhd_io_num = -1;
            this->buss_config.intr_flags = 0;  //set interrupt flags to 0

            esp_err_t ret = spi_bus_initialize(HSPI_HOST, &this->buss_config, 1);
            if (ret != ESP_OK) {
                printf("Failed to initialize SPI bus. Error code: %d\n", ret);
                return;
            }

            // Device interface config
            memset(&this->device_config, 0, sizeof(this->device_config));  // Zero out the structure
            this->device_config.clock_speed_hz = 4e6; // 4MHz clock out
            this->device_config.mode = 0;
            this->device_config.spics_io_num = CS;
            this->device_config.queue_size = 7; // Size of transaction queue

            ret = spi_bus_add_device(HSPI_HOST, &this->device_config, &this->handle);
            if (ret != ESP_OK) {
                printf("Failed to add SPI device. Error code: %d\n", ret);
                return;
            }
        }


        ~SPIBus(){
            spi_bus_remove_device(this->handle);
            spi_bus_free(HSPI_HOST);
        }

        void transfer(void * command, void * response, size_t length){
            
            memset(&this->transaction, 0, sizeof(this->transaction));
            
            this->transaction.length = 8 * length;
            this->transaction.tx_buffer = command;
            this->transaction.rx_buffer = response;
            spi_device_transmit(this->handle, &this->transaction);

        }

};