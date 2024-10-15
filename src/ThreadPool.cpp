#include "ThreadPool.hpp"

MyPool::MyPool(size_t num) : stop(false){


    for(size_t i=0; i<num; i++){
        threads_.emplace_back([this]{
            //each thread has this lambda function
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(qMutex);
                    cv.wait(lock, [this]{
                                        return stop || !tasks_.empty(); 
                                        });
                    //if there no tasks or we're leaving then dont do anything
                    if(stop && tasks_.empty()){
                        return;
                    }
                    //else get the task method thats at the front of the queue
                    task = std::move(tasks_.front());
                    //remove that method from the queue
                    tasks_.pop();
                    //we're done using a shared thngy so we let it go
                }
                task();
            } 
        });
    }

}

MyPool::~MyPool(){
    
    //when closing the application
    //destructor called, we stop the threads
    {
        std::unique_lock<std::mutex> lock(qMutex);
        stop = true;
    }
        //notify all of them that we want to close
        cv.notify_all();

    //then we do this so they..idk...leave?
    for(std::thread &t : threads_){
        t.join();
    }

}

void MyPool::enqueue(std::function<void()> task){

        {
            std::unique_lock<std::mutex> lock(qMutex);
            tasks_.emplace(std::move(task));
        }
        //notify a waiting thread that a task is available
        cv.notify_one();
}

std::string MyPool::getThreadId(){
    //convert weird return type id to string
    auto myId = std::this_thread::get_id();
    std::stringstream ss;
    ss << myId;
    std::string myStr = ss.str();
    return myStr;
}