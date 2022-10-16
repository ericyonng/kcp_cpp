
#include <pch.h>
#include <kcp/ConsoleConfigMgr.h>
#include <kcp/util/FileUtil.h>
#include <kcp/LibIniFile.h>
#include <kcp/util/SystemUtil.h>
#include <kcp/util/DirectoryUtil.h>
#include <kcp/LibString.h>
#include <kcp/ConsoleConfigTemplate.h>

KCP_CPP_BEGIN

ConsoleConfigMgr::ConsoleConfigMgr()
    :_ini(NULL)
    ,_front{0}
    ,_back{0}
{

}

ConsoleConfigMgr::ConsoleConfigMgr(bool isInit)
    :_ini(NULL)
    ,_front{0}
    ,_back{0}
{
    // 获取当前程序绝对路径
    LibString path;
    SystemUtil::GetProgramPath(true, path);
    const auto &rootDir = DirectoryUtil::GetFileDirInPath(path) + ROOT_DIR_INI_SUB_DIR;
    (Init("ConsoleConfig.ini", rootDir.c_str()));
}

ConsoleConfigMgr::~ConsoleConfigMgr()
{
    Close();
}

// 放在程序根目录ini子目录下
ConsoleConfigMgr *ConsoleConfigMgr::GetInstance()
{
    static std::shared_ptr<ConsoleConfigMgr> s_consoleMgr(new ConsoleConfigMgr(true));
    return s_consoleMgr.get();
}

bool ConsoleConfigMgr::Init(const Byte8 *cfgFileName, const Byte8 *dirName)
{
    if((_ini))
    {
        return false;
    }

    LibString fileName = dirName;
    fileName += cfgFileName;

    // 文件是否存在 不存在则创建
    if((!FileUtil::IsFileExist(fileName.c_str())))
    {
        if(fileName.empty())
        {
            return false;
        }

        // 获取路径
        const auto &fileDir = DirectoryUtil::GetFileDirInPath(fileName);
        // 创建路径
        if(!DirectoryUtil::CreateDir(fileDir))
        {
            return false;
        }

        auto fp = FileUtil::OpenFile(fileName.c_str(), true);
        const auto &content = ConsoleConfigTemplate::GetConsoleIniContent();
        FileUtil::WriteFile(*fp, content.data(), content.size());
        FileUtil::FlushFile(*fp);
        FileUtil::CloseFile(*fp);
    }

    _ini = new LibIniFile();

    if(!_ini->Init(fileName.c_str(), false))
    {
        return false;
    }

    // 初始化前景色
    Int64 result;
    LibString keyName = "Black";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._black = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);
    
    keyName = "Red";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._red = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Green";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", "Green", result))
    {
        return false;
    }
    _front._green = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Blue";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._blue = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Yellow";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._yellow = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Purple";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        CRYSTAL_TRACE("CheckReadInt Win32FrontConsoleColor %s FAIL", keyName.c_str());
        return false;
    }
    _front._purple = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Cyan";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._cyan = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "White";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._white = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Gray";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._gray = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "LightYellow";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._lightYellow = static_cast<Int32>(result);
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Highlight";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._highLight = static_cast<Int32>(result);  
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "FrontDefault";
    if(!_ini->CheckReadInt("Win32FrontConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _front._frontDefault = static_cast<Int32>(result);  
    _frontNameRefColor[keyName] = static_cast<Int32>(result);

    // 初始化背景色
    keyName = "Black";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._black = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Red";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._red = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Green";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._green = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Blue";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._blue = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);


    keyName = "Yellow";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._yellow = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Purple";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._purple = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Cyan";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._cyan = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "White";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._white = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "Highlight";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._highLight = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    keyName = "BackDefault";
    if(!_ini->CheckReadInt("Win32BackConsoleColor", keyName.c_str(), result))
    {
        return false;
    }
    _back._backDefault = static_cast<Int32>(result); 
    _backNameRefColor[keyName] = static_cast<Int32>(result);

    return true;
}

void ConsoleConfigMgr::Close()
{
    if(_ini)
    {
      delete _ini;
    }

    _ini = NULL;
}

const Int32 ConsoleConfigMgr::GetFrontColor(const Byte8 *colorName) const
{
    return _frontNameRefColor.find(colorName)->second;
}

const Int32 ConsoleConfigMgr::GetBackColor(const Byte8 *colorName) const
{
    return _backNameRefColor.find(colorName)->second;
}

KCP_CPP_END