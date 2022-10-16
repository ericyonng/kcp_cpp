
#include "pch.h"
#include "Handler/HelloWorldReqHandler.h"
#include "DevelopmentKit/DevelopmentKit.h"

void HelloWorldReqHandler::ProcessMessage(KcpChannel *channel, LibPacket *packet) 
{
    auto coder = packet->GetCoder();
    auto req = LibCast<HelloWorldReq>(coder);
    g_Log->Info(LOGFMT_OBJ_TAG("ProcessMessage HelloWorldReq content:%s"), req->_content.c_str());

    auto res = new HelloWorldRes;
    res->_content = req->_content;
    channel->Send(Opcodes::HelloWorldRes, res, packet->GetPacketId());
}