#pragma once
#include <condition_variable>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <iostream>
#include <functional>
#include <sstream>
/*
pool of threads that're created when app is first launched
they share tasks, tasks are queued and...yeah
provides less overhead cos threads dont get destroyed and creted everytime

*/
class MyPool {

    public:
        //create threads in the constructor
        MyPool(size_t numThreads = std::thread::hardware_concurrency());
        //destroy threads in the destructor
        ~MyPool();
        //push tasks into queue, send a sisgnal to threads that job arrived
        void enqueue(std::function<void()> task);
        std::string getThreadId();
    private:
        std::vector<std::thread> threads_;
        std::queue<std::function<void()>> tasks_;
        std::mutex qMutex;
        //condition_variable cv_;        #
        std::condition_variable cv;
        bool stop;

};