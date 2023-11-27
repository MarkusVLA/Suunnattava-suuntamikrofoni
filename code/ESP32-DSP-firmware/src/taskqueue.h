#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <Arduino.h>

template <typename T>
class TaskQueue {
private:
    struct Node {
        T task_;
        int priority_;
        Node* next_;

        Node(T d, int p, Node* n = nullptr) : task_(d), priority_(p), next_(n) {}
    };

    Node* head_;
    Node* tail_;
    size_t size_;

public:
    TaskQueue() : head_(nullptr), tail_(nullptr), size_(0) {}
    ~TaskQueue();

    void enqueue(const T& item, int priority);
    T dequeue();
    bool isEmpty() const;
    size_t getSize() const;
};

template <typename T>
TaskQueue<T>::~TaskQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

template <typename T>
void TaskQueue<T>::enqueue(const T& item, int priority) {
    Node* newNode = new Node(item, priority);
    if (isEmpty() || head_->priority_ > priority) {
        newNode->next_ = head_;
        head_ = newNode;
    } else {
        Node* current = head_;
        while (current->next_ != nullptr && current->next_->priority_ <= priority) {
            current = current->next_;
        }
        newNode->next_ = current->next_;
        current->next_ = newNode;
    }
    if (newNode->next_ == nullptr) {
        tail_ = newNode;
    }
    size_++;
}

template <typename T>
T TaskQueue<T>::dequeue() {
    if (isEmpty()) {
        return nullptr;
    }
    Node* temp = head_;
    T item = head_->task_;
    head_ = head_->next_;
    if (head_ == nullptr) {
        tail_ = nullptr;
    }
    delete temp;
    size_--;
    return item;
}

template <typename T>
bool TaskQueue<T>::isEmpty() const {
    return head_ == nullptr;
}

template <typename T>
size_t TaskQueue<T>::getSize() const {
    return size_;
}

#endif // TASKQUEUE_H
