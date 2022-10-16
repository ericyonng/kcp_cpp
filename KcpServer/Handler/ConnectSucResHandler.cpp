

#include "pch.h"
#include "Handler/ConnectSucResHandler.h"
#include "DevelopmentKit/DevelopmentKit.h"
#include "KcpServer.h"

static std::atomic<Int64> s_packetId {0};

void ConnectSucResHandler::ProcessMessage(KcpChannel *channel, LibPacket *packet) 
{
    auto coder = packet->GetCoder();
    auto res = LibCast<ConnectSucRes>(coder);
    // g_Log->Info(LOGFMT_OBJ_TAG("ProcessMessage ConnectSucRes channel:%s"), channel->ToString().c_str());

    auto timerMgr = g_Server->GetTimerMgr();
    auto newTimer = new KCP_CPP_NS::LibTimer(timerMgr);
    auto sessionId = channel->GetSesssionId();
    auto timerDeleg = [this, sessionId](KCP_CPP_NS::LibTimer *timer)-> void
    {
        auto channel = g_Server->GetChannel(sessionId);
        if(channel)
        {
            // g_Log->Info(LOGFMT_OBJ_TAG("send HelloWorldReq channel:%s"), channel->ToString().c_str());
            auto req = new HelloWorldReq;
            req->_content = "Hello world hello KCP!";

            channel->Send(Opcodes::HelloWorldReq, req, ++s_packetId);
        }
    };

    auto delg = CREATE_CLOSURE_DELEGATE(timerDeleg, void, KCP_CPP_NS::LibTimer *);
    newTimer->SetTimeOutHandler(delg);
    newTimer->Schedule(2000);
}