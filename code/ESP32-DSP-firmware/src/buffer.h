// Buffer array structure for reading and processing data.

#ifndef BUFFER_H
#define BUFFER_H

#include <Arduino.h>
#include <iostream>

class Buffer{

private:

    int xDim;
    int yDim;
    uint16_t ** data;
    
    void freeMem(void); // We will call this in the destructor.

public:
    
    Buffer(int x_dim, int y_dim);
    ~Buffer(); // free memory here
    
    int getXDim(void);
    int getYDim(void);
    void initializeBuffer(void); // initialize memory and create buffer
    uint16_t getValue(int x, int y) const; // get individual value in the buffer
    void setValue(uint16_t val, int x, int y); // set individual value in the buffer
    void copyBuffer(Buffer &bufferToCopy); // copies all values from argument buffer data buffer to instance data
    void zero(void); // Zero out entire buffer data.
    const uint16_t * getCollumn(int col) const;// get a pointer to the collumn of the buffer

    friend std::ostream& operator<<(std::ostream &os, Buffer &buffer); // print a sample of the buffer to serial out

};

#endif // BUFFER_H