/*!
 *  MIT License
 *  
 *  Copyright (c) 2020 ericyonng<120453674@qq.com>
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 * 
 * Date: 2020-12-07 00:17:59
 * Author: Eric Yonng
 * Description: 
*/

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_ITASK_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_ITASK_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

class ITask
{
public:
    virtual void Run() = 0;
    virtual void Release() = 0;
    virtual void SetArg(void *arg) { _arg = arg; }
    virtual void *GetArg() { return _arg; }

protected:
    ITask() { _arg = NULL; }
    virtual ~ITask() {}

protected:
    void *_arg;
};

KCP_CPP_END

#endif
