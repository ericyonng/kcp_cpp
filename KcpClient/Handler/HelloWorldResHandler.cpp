
#include "pch.h"
#include "Handler/HelloWorldResHandler.h"
#include "DevelopmentKit/DevelopmentKit.h"

void HelloWorldResHandler::ProcessMessage(KcpChannel *channel, LibPacket *packet) 
{
    auto coder = packet->GetCoder();
    auto res = LibCast<HelloWorldRes>(coder);

    const auto nowMs = KCP_CPP_NS::LibTime::NowMilliTimestamp();
    const auto diff = nowMs - res->_startTime;
    // g_Log->Custom("helllo res cost ms:%lld", diff);
//    g_Log->Info(LOGFMT_OBJ_TAG("ProcessMessage HelloWorldRes content:%s, packetId:%lld")
//                , res->_content.c_str(), packet->GetPacketId());
}
