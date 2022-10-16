
#ifndef __KCP_CPP_KCP_SERVER_HANDLER_CONNECT_SUC_RES_HANDLER_H__
#define __KCP_CPP_KCP_SERVER_HANDLER_CONNECT_SUC_RES_HANDLER_H__

#pragma once

#include "IMessageHandler.h"

class ConnectSucResHandler :public IMessageHandler
{
public:
    ConnectSucResHandler() {}
    ~ConnectSucResHandler() {}

    virtual void Release() override{ delete this; }
    
    virtual void ProcessMessage(KcpChannel *channel, LibPacket *packet) override;
};

#endif
