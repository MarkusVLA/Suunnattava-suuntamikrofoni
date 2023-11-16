/**
 * @file taskmanager.hpp
 * @brief Task manager class to manage tasks
 * @version 0.1
 * 
 * 
 */


#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include <string>
#include <queue>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


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
    std::priority_queue<TaskConfig, std::vector<TaskConfig>, ComparePriority> taskQueue_;

public:

    TaskManager() { }
    ~TaskManager() { }
    
    void addTask(const TaskConfig &task){
        taskQueue_.push(task);
    }
    
    void startTask() {
        while (!taskQueue_.empty()){
            // Get highest priority task:
            const TaskConfig &config = taskQueue_.top();
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
            taskQueue_.pop();
        }   
    }

};

#endif // TASKMANAGER_HPP