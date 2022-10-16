

#include "pch.h"
#include "Handler/HeartbeatReqHandler.h"
#include "DevelopmentKit/DevelopmentKit.h"
#include "KcpServer.h"

static std::atomic<Int64> s_packetId {0};

void HeartbeatReqHandler::ProcessMessage(KcpChannel *channel, LibPacket *packet) 
{
    auto coder = packet->GetCoder();
    auto req = LibCast<HeartbeatReq>(coder);
    // g_Log->Info(LOGFMT_OBJ_TAG("ProcessMessage HeartbeatReq channel:%s"), channel->ToString().c_str());
}