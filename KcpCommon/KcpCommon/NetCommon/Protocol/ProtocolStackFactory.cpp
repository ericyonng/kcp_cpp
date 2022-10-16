
#include "pch.h"
#include "KcpCommon/NetCommon/Protocol/ProtocolStackFactory.h"
#include "KcpCommon/NetCommon/Protocol/ProtocolStackType.h"
#include "KcpCommon/NetCommon/Protocol/ServerClientProtocolStack.h"


IProtocolStack *ProtocolStackFactory::Create(Int32 protocolStackType)
{
    switch (protocolStackType)
    {
        case ProtocolStackType::SERVER_CLIENT:
        {// 服务端客户端协议栈
            return new ServerClientProtocolStack;
        }
        break;
        default:
        {
            g_Log->Error(LOGFMT_NON_OBJ_TAG(ProtocolStackFactory, "unknown protocol stack type:%d"), protocolStackType);
        }
        break;
    }

    return NULL;
}
