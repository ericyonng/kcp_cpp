

#include "pch.h"
#include "Handler/ConnectReqHandler.h"
#include "DevelopmentKit/DevelopmentKit.h"
#include "KcpServer.h"

static std::atomic<Int64> s_packetId {0};

void ConnectReqHandler::ProcessMessage(KcpChannel *channel, LibPacket *packet) 
{
    auto coder = packet->GetCoder();
    auto req = LibCast<ConnectReq>(coder);
    // g_Log->Info(LOGFMT_OBJ_TAG("ProcessMessage ConnectReq channel:%s"), channel->ToString().c_str());

    auto res = new ConnectSucRes;
    channel->Send(Opcodes::ConnectSucRes, res, packet->GetPacketId());
}