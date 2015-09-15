
#ifndef TIMER_H_
#define TIMER_H_

#include<chrono>
#include"stdint.h"

namespace gl {
    class timer
    {
    public:
        timer():d_begin(std::chrono::high_resolution_clock::now()){
            
        }
        void reset() {
            d_begin = std::chrono::high_resolution_clock::now();
        }
        uint64_t elapsed() const{
            return __elapsed<std::chrono::milliseconds>();
        }
        uint64_t elapsed_nanoseconds() const{
            return __elapsed<std::chrono::nanoseconds>();
        }
        uint64_t elapsed_microseconds() const{
            return __elapsed<std::chrono::microseconds>();
        }
        uint64_t elapsed_milliseconds() const{
            return __elapsed<std::chrono::milliseconds>();
        }
        uint64_t elapsed_seconds() const{
            return __elapsed<std::chrono::seconds>();
        }
        uint64_t elapsed_minutes() const{
            return __elapsed<std::chrono::minutes>();
        }
        uint64_t elapsed_hours() const{
            return __elapsed<std::chrono::hours>();
        }
    private:
        template<typename T>
        uint64_t __elapsed() const{
            return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - d_begin).count();
        }
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> d_begin;
    };
}

#endif
