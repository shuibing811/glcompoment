#ifndef NONCOPYABLE_H_ 
#define NONCOPYABLE_H_

namespace gl {
    class noncopyable
    {
    public:
        noncopyable() = delete;
        noncopyable(const noncopyable& other) = delete;
        noncopyable(noncopyable&& other) = delete;
        noncopyable& operator=(const noncopyable& other) = delete;
    };
}

#endif
