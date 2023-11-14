/**
 * @brief Serial stream class for easier serial data transfer
 * 
 */


#ifndef SERIALSTEAMBUF_HPP
#define SERIALSTEAMBUF_HPP

#include <iostream>
#include <Arduino.h>

class SerialStreambuf : public std::streambuf {

protected:

    virtual int_type overflow(int_type v) {
        if (v == '\n') {
            Serial.write('\r');
        }
        Serial.write(v);
        return v;
    }

    virtual int sync() {
        return 0; 
    }
};



#endif // SERIALSTREAMBUF_HPP