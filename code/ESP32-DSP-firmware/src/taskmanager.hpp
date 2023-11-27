/**
 * @file taskmanager.hpp
 * @brief Task manager class to manage tasks
 * @version 0.1
 * 
 * 
 */


#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include "taskqueue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


struct TaskConfig {

    void (*taskFunction_)(void*);
    const char* taskName_;
    unsigned int stackSize_;
    void * taskParams_;
    unsigned int taskPriority_;
    unsigned int core_;

    TaskConfig(

        void (*func)(void*) = nullptr,
        const char *name = nullptr, 
        unsigned int stackSize = 1000,
        void * taskParams = nullptr,
        unsigned int taskPriority = 0,
        int core = 1
        ): taskFunction_(func), taskName_(name), stackSize_(stackSize), taskParams_(taskParams), taskPriority_(taskPriority), core_(core){ }

};


struct ComparePriority {

    bool operator()(const TaskConfig &a, const TaskConfig &b){
        return a.taskPriority_ < b.taskPriority_;
    }

};

class TaskManager {

private:
    // Hold task in a priority queue.
    TaskQueue<TaskConfig> taskQueue_;

public:

    TaskManager() { }
    ~TaskManager() { }
    
    void addTask(const TaskConfig &task, int priority) {
        taskQueue_.enqueue(task, priority);
    }

    void startTask() {
        while (!taskQueue_.isEmpty()){
            // Get highest priority task:
            const TaskConfig &config = taskQueue_.dequeue();
            // start task
            xTaskCreatePinnedToCore(
                config.taskFunction_,
                config.taskName_,
                config.stackSize_,
                config.taskParams_,
                config.taskPriority_,
                nullptr,
                config.core_
            );
        }   
    }

};

#endif // TASKMANAGER_HPP