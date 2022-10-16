
#ifndef __KCP_CPP_KCP_COMMON_OPCODES_H__
#define __KCP_CPP_KCP_COMMON_OPCODES_H__

#pragma once

class Opcodes
{
public:
    enum ENUMS
    {
        ConnectReq = 1,  // 连接申请
        ConnectSucRes = 2,  // 成功连入
        
        HelloWorldReq = 3,  // hello world req
        HelloWorldRes = 4,  // hello world res

        TestGateWayReq = 9900,
        TestGateWayRes = 9901,
        HeartbeatReq = 5,  // 心跳
    };
};

#endif
