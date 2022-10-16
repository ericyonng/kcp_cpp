
#ifndef __KCP_CPP_KCP_SERVER_HANDLER_CONNECT_REQ_HANDLER_H__
#define __KCP_CPP_KCP_SERVER_HANDLER_CONNECT_REQ_HANDLER_H__

#pragma once

#include "IMessageHandler.h"

class ConnectReqHandler :public IMessageHandler
{
public:
    ConnectReqHandler() {}
    ~ConnectReqHandler() {}

    virtual void Release() override{ delete this; }
    
    virtual void ProcessMessage(KcpChannel *channel, LibPacket *packet) override;
};

#endif