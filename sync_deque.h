#ifndef SYNC_DEQUE_H_
#define SYNC_DEQUE_H_

#include "circular_buffer.h"
#include <condition_variable>
#include <mutex>

namespace gl{
    template<typename T,bool is_full_block=true>
    class sync_deque
    {
    public:
        typedef gl::circular_buffer<T>       container_type;
        typedef typename container_type::value_type     value_type;
        typedef typename container_type::reference      reference;
        typedef typename container_type::pointer        pointer;
        typedef typename container_type::size_type      size_type;
        typedef typename container_type::difference_type    difference_type;
        typedef typename container_type::param_value_type   param_value_type;
        typedef typename container_type::rvalue_type        rvalue_type;
    public:
        sync_deque(size_type capacity)
            :d_container(capacity){
            
        }
        template<typename ... Args>
        void push_back(Args ... args){
            std::unique_lock<std::mutex> lock(d_mutex);
            if(is_full_block){
                d_is_not_full.wait(lock,[this]()->bool{ return !d_container.full();});
            }
            d_container.emplace_back(std::forward<Args>(args)...);
            lock.unlock();
            d_is_not_empty.notify_all();
        }
        template<typename ... Args>
        void push_front(Args ... args){
            std::unique_lock<std::mutex> lock(d_mutex);
            if(is_full_block){
                d_is_not_full.wait(lock,[this]()->bool{ return !d_container.full(); });
            }
            d_container.emplace_front(std::forward<Args>(args)...);
            lock.unlock();
            d_is_not_empty.notify_all();
        }
        rvalue_type pop_front(){
            std::unique_lock<std::mutex> lock(d_mutex);
            d_is_not_empty.wait(lock,[this]()->bool{ return !d_container.empty(); });
            reference value = d_container.front();
            d_container.pop_front();
            lock.unlock();
            if(is_full_block){
                d_is_not_full.notify_all();
            }
            return std::move(value);
        }
        rvalue_type pop_back(){
            std::unique_lock<std::mutex> lock(d_mutex);
            d_is_not_empty.wait(lock,[this]()->bool{ return !d_container.empty(); });
            reference value = d_container.back();
            d_container.pop_back();
            lock.unlock();
            if(is_full_block){
                d_is_not_full.notify_all();
            }
            return std::move(value);
        }
    private:
        container_type          d_container;
        std::mutex              d_mutex;
        std::condition_variable d_is_not_empty;
        std::condition_variable d_is_not_full;
    };
}

#endif
