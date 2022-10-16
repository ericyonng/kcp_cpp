

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_ASYN_TIME_DATA_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_ASYN_TIME_DATA_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/util/BitUtil.h>

KCP_CPP_BEGIN

class TimeData;

class AsynOpType
{
public:
    enum
    {
        OP_NONE = 0,            // 无效
        OP_REGISTER = 1,        // 注册
        OP_UNREGISTER = 2,      // 反注册
        OP_DESTROY = 3,         // 销毁
    };
};

class AsynTimeData
{

public:
    AsynTimeData(TimeData *data);
    void Release();

public:
    void MaskRegister(Int64 expireTime, Int64 newPeriod);
    void MaskUnRegister();
    void MaskDestroy();
    void Reset();
    bool IsMaskRegister() const;

public:
    UInt32 _flag;                   // 操作位标记 AsynOpType
    Int64 _newExpiredTime;          // 当前过期时间 微妙 修改数据时候使用
    Int64 _newPeriod;               // 定时周期 微妙    修改数据时候使用
    TimeData *_data;                // 原始的定时数据
};

inline AsynTimeData::AsynTimeData(TimeData *data)
    :_flag(AsynOpType::OP_NONE)
    ,_newExpiredTime(0)
    ,_newPeriod(0)
    ,_data(data)
{

}

inline void AsynTimeData::Release()
{
    delete this;
}

inline void AsynTimeData::MaskRegister(Int64 expireTime, Int64 newPeriod)
{
    // 移除删除与修改
    _flag = BitUtil::Set(_flag, AsynOpType::OP_REGISTER);
    _newExpiredTime = expireTime;
    _newPeriod = newPeriod;
}

inline void AsynTimeData::MaskUnRegister()
{
    // 移除添加与修改
    _flag = BitUtil::Clear(_flag, AsynOpType::OP_REGISTER);
    _flag = BitUtil::Set(_flag, AsynOpType::OP_UNREGISTER);
}

inline void AsynTimeData::MaskDestroy()
{
    _flag = BitUtil::Clear(_flag, AsynOpType::OP_REGISTER);
    _flag = BitUtil::Clear(_flag, AsynOpType::OP_UNREGISTER);
    _flag = BitUtil::Set(_flag, AsynOpType::OP_DESTROY);
}

inline void AsynTimeData::Reset()
{
    _flag = 0;
}

inline bool AsynTimeData::IsMaskRegister() const
{
    return BitUtil::IsSet(_flag, AsynOpType::OP_REGISTER);
}

KCP_CPP_END


#endif


