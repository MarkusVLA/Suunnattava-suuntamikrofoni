
// playbackQueue.h

#include <Arduino.h>
#include <iostream>

#ifndef PLAYBACKQUEUE_H
#define PLAYBACKQUEUE_H


struct Node {

    float val;
    Node * next;

};



class playbackQueue {

private:

    int len;
    Node * head;
    Node * tail;

public:

    playbackQueue();
    ~playbackQueue();

    float popHead(void);             // Return front Node of the queue
    void pushBack(float value);     // Add value to back of the queue
    bool isEmpty() const;           // Check if the queue is empty
    int getSize() const;            // Return the size of the queue
    friend std::ostream& operator<<(std::ostream &os, playbackQueue &buffer); // print a sample of the queue to serial out

};


#endif // PLAYBACKQUEUE_H