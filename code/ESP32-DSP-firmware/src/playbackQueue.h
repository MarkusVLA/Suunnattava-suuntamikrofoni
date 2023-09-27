
// playbackQueue.h

#include <Arduino.h>

#ifndef PLAYBACKQUEUE_H
#define PLAYBACKQUEUE_H


typedef struct Node {

    float val;
    Node * next;

} Node;



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

};


#endif // PLAYBACKQUEUE_H