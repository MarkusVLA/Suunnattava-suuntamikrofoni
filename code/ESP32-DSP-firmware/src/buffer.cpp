#include "buffer.h"

Buffer::Buffer(int x_dim, int y_dim){
    xDim = x_dim;
    yDim = y_dim;

    initializeBuffer();
}

Buffer::~Buffer(){
    freeMem(); // Free memory when object goes out of scope.
} 

void Buffer::freeMem(void){

    // function to free memory
    for (int i = 0; i < getXDim(); i++){
        delete [] data[i];
    }
    delete [] data;

}

int Buffer::getXDim(void){
    return xDim;
}

int Buffer::getYDim(void){
    return yDim;
}

void Buffer::initializeBuffer(void){
    // initialize memory and create buffer
    data = new uint16_t * [xDim];
    for (int i = 0; i < xDim; i++){
        data[i] = new uint16_t [yDim];
    }
}

uint16_t Buffer::getValue(int x, int y) const{
    return data[x][y];

} 

void Buffer::setValue(uint16_t val, int x, int y){
    data[x][y] = val;
} 

const uint16_t * Buffer::getCollumn(int col) const{
    if (col < 0 || col >= xDim) {
        return nullptr;
    }

    // get a pointer to the collumn of the buffer
    return data[col];

}


void Buffer::copyBuffer(Buffer &bufferToCopy){
    // copies all values from argument buffer data buffer to instance data
    for (int i = 0; i < xDim; i++){
        memcpy(data[i], bufferToCopy.getCollumn(i), yDim * sizeof(uint16_t));
    }

} 

void Buffer::zero(void){

    for (int i = 0; i < xDim; i++){
        memset(data[i], 0, yDim * sizeof(uint16_t));
    }

} // Zero out entire buffer data.