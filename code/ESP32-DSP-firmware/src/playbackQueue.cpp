#include "playbackQueue.h"

playbackQueue::playbackQueue():len(0), head(nullptr), tail(nullptr) { }

playbackQueue::~playbackQueue() {
    while (!isEmpty()){
        popHead();
    }
}

bool playbackQueue::isEmpty() const {
    return head == nullptr;
}

int playbackQueue::getSize() const {
    return len;
}

float playbackQueue::popHead(void) {
    if (isEmpty()) {
        return -1.0f;
    }

    Node * temp = head;

    float tempVal = temp->val;
    head = temp->next;
    delete temp;
    len--;

    if (isEmpty()) {
        tail = nullptr;
    }

    return tempVal;
}


void playbackQueue::pushBack(float value){

    Node * newNode = new Node;

    newNode->val = value;
    newNode->next = nullptr;

    if (isEmpty()){
        head = newNode;
        tail = newNode;
    }
    
    else {
        tail->next = newNode;
        tail = newNode;
    }

    len++;
}


std::ostream& operator<<(std::ostream &os, playbackQueue &buffer) {
    os << buffer.head->val << std::endl; // Peek at the buffers first value.
    return os;
}
