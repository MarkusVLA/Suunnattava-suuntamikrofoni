
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "spi.hpp"
#include <vector>

#define BLINK_GPIO GPIO_NUM_2

class Pin {

    private:

        gpio_num_t pin_number;
        gpio_mode_t pin_mode;

    public:

        Pin(int pin_number, gpio_mode_t pin_mode){

            this->pin_number = (gpio_num_t) pin_number;
            this->pin_mode = pin_mode;

            gpio_reset_pin(this->pin_number);

            gpio_set_direction(this->pin_number, this->pin_mode);

        }

        ~Pin(){

            gpio_reset_pin(this->pin_number);

        }

        void set_value(int value){
            gpio_set_level(this->pin_number, value);
        }

        int get_value() const {
            return gpio_get_level(this->pin_number);
        }
};