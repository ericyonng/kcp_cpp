
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_CONSOLE_CONFIG_MGR_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_CONSOLE_CONFIG_MGR_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

class LibIniFile;

// 前景色
struct ConsoleFrontColor
{
    Int32 _black;
    Int32 _red;
    Int32 _green;
    Int32 _blue;
    Int32 _yellow;
    Int32 _purple;
    Int32 _cyan;
    Int32 _white;
    Int32 _gray;
    Int32 _lightYellow;
    Int32 _highLight;
    Int32 _frontDefault;
};

// 背景色
struct ConsoleBackColor
{
    Int32 _black;
    Int32 _red;
    Int32 _green;
    Int32 _blue;
    Int32 _yellow;
    Int32 _purple;
    Int32 _cyan;
    Int32 _white;
    Int32 _highLight;
    Int32 _backDefault;
};

class ConsoleConfigMgr
{
public:
    ConsoleConfigMgr();
    ConsoleConfigMgr(bool isInit);
    ~ConsoleConfigMgr();

public: 
    // 放在程序根目录ini子目录下
    static ConsoleConfigMgr *GetInstance();
    // 绝对路径
    bool Init(const Byte8 *cfgFileName = "ConsoleConfig.ini", const Byte8 *dirName = NULL);
    void Close();

    const ConsoleFrontColor &GetFrontColor() const;
    const ConsoleBackColor &GetBackColor() const;

    const Int32 GetFrontColor(const Byte8 *colorName) const;
    const Int32 GetBackColor(const Byte8 *colorName) const;

    bool IsInit() const;

private:
    LibIniFile *_ini;
    ConsoleFrontColor _front;   // 前景色
    std::map<LibString, Int32> _frontNameRefColor;

    ConsoleBackColor _back;     // 背景色
    std::map<LibString, Int32> _backNameRefColor;
};

inline const ConsoleFrontColor &ConsoleConfigMgr::GetFrontColor() const
{
    return _front;
}

inline const ConsoleBackColor &ConsoleConfigMgr::GetBackColor() const
{
    return _back;
}

inline bool ConsoleConfigMgr::IsInit() const
{
    return _ini != NULL;
}


KCP_CPP_END

#endif
