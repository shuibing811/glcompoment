//
//  noncopyable.h
//  Test
//
//  Created by 郭磊 on 15/9/14.
//  Copyright (c) 2015年 郭磊. All rights reserved.
//

#ifndef Test_noncopyable_h
#define Test_noncopyable_h

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
