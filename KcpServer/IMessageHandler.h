
#ifndef __KCP_CPP_KCP_SERVER_IMESSAGE_HANDLER_H__
#define __KCP_CPP_KCP_SERVER_IMESSAGE_HANDLER_H__

#pragma once

class KcpChannel;
class LibPacket;

class IMessageHandler
{
public:
    IMessageHandler(){}
    virtual ~IMessageHandler() {}
    virtual void Release() = 0;
    
    virtual void ProcessMessage(KcpChannel *channel, LibPacket *packet) = 0;
};

#endif