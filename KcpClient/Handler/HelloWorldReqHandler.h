
#ifndef __KCP_CPP_KCP_CLIENT_HANDLER_HELLO_WORLD_REQ_HANDLER_H__
#define __KCP_CPP_KCP_CLIENT_HANDLER_HELLO_WORLD_REQ_HANDLER_H__

#pragma once

#include "IMessageHandler.h"

class HelloWorldReqHandler :public IMessageHandler
{
public:
    HelloWorldReqHandler() {}
    ~HelloWorldReqHandler() {}

    virtual void Release() override{ delete this; }
    
    virtual void ProcessMessage(KcpChannel *channel, LibPacket *packet) override;
};

#endif
