
#include "pch.h"
#include "Handler/HelloWorldResHandler.h"
#include "DevelopmentKit/DevelopmentKit.h"

void HelloWorldResHandler::ProcessMessage(KcpChannel *channel, LibPacket *packet) 
{
    auto coder = packet->GetCoder();
    auto res = LibCast<HelloWorldRes>(coder);
    g_Log->Info(LOGFMT_OBJ_TAG("ProcessMessage HelloWorldRes content:%s, packetId:%lld")
                , res->_content.c_str(), packet->GetPacketId());
}
