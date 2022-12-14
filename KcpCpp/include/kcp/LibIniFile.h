

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_INI_FILE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_INI_FILE_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>
#include <kcp/Locker.h>

KCP_CPP_BEGIN

class LibIniFile
{
    
public:
    LibIniFile();
    virtual ~LibIniFile();

public:
    bool Init(const Byte8 *path, bool createIfNotExist = true);
    void Clear();
    void Flush();
    const LibString &GetPath() const;
    const LibString &GetFileContent() const;

    void Lock();
    void Unlock();

    bool ReadStr(const Byte8 *segmentName, const Byte8 *keyName, const Byte8 *defaultStr, LibString &strOut) const;
    bool ReadStr(const Byte8 *segmentName, const Byte8 *keyName, LibString &strOut) const;
    
    Int64 ReadInt(const Byte8 *segmentName, const Byte8 *keyName, Int64 defaultInt) const;
    UInt64 ReadUInt(const Byte8 *segmentName, const Byte8 *keyName, UInt64 defaultInt) const;
    bool CheckReadInt(const Byte8 *segmentName, const Byte8 *keyName, Int64 &result) const;
    bool CheckReadUInt(const Byte8 *segmentName, const Byte8 *keyName, UInt64 &result) const;

    bool CheckReadNumber(const Byte8 *segmentName, const Byte8 *keyName, Int64 &result) const;
    bool CheckReadNumber(const Byte8 *segmentName, const Byte8 *keyName, UInt64 &result) const;
    bool CheckReadNumber(const Byte8 *segmentName, const Byte8 *keyName, Double &result) const;

    bool WriteStr(const Byte8 *segmentName, const Byte8 *keyName, const Byte8 *wrStr);
    bool WriteNumber(const Byte8 *segmentName, const Byte8 *keyName, UInt64 number);
    bool WriteNumber(const Byte8 *segmentName, const Byte8 *keyName, Int64 number);
    bool WriteNumber(const Byte8 *segmentName, const Byte8 *keyName, Double number);

    bool HasCfgs(const Byte8 *segmentName) const;
    bool HasCfgs() const;
    bool ChangeLineBetweenSegs();
    bool WriteFileHeaderAnnotation(const LibString &content);   // 内容只会在第一个segment的位置插入

    // 段下所有配置
    const std:: map<LibString, LibString> *GetSegmentCfgs(const Byte8 *segmentName) const;

private:
    bool _Init();
    bool _LoadAllCfgs();

    bool _ReadStr(const Byte8 *segmentName, const Byte8 *keyName, LibString &strOut) const;
    bool _WriteStr(const Byte8 *segmentName, const Byte8 *keyName, const Byte8 *wrStr);

    // 插入新行数据
    bool _InsertNewLineData(Int32 line, const LibString &segment, const LibString &key, const LibString &value); // 会重新加载配置，容器迭代器全部失效
    bool _InsertNewLineData(Int32 line, const LibString &content); // 向某一行插入任意数据
    // 更新配置
    void _UpdateIni(bool isUpdateMemCache = false);
    // 更新内存缓存
    UInt64 _UpdateFileMemCache(FILE &fp);

    // 读取到有效的数据
    void _OnReadValidData(const LibString &validContent
                          , Int32 contentType
                          , Int32 line
                          , LibString &curSegment
                          , std::map<LibString, LibString> *&curKeyValues);

    // 段的所包含的键值对的最大行
    Int32 _GetSegmentKeyValueMaxValidLine(const LibString &segment) const;  // 返回-1该段不存在
    Int32 _GetSegmentLineByLoop(Int32 index) const; // 获取第index个segment的行号

private:
    Locker _lock;
    LibString  _filePath;
    LibString _fileContent;                                                         // 配置文件内容

    bool _isDirtied;
    Int32 _maxLine;                                                                 // 行号最小值1
    std:: map<Int32,     LibString> _lineRefContent;                                // 每一行的元数据,行号最小值从1开始
    std:: map<LibString, Int32> _segOrKeyRefLine;                                   // seg对应的行号或者seg-key所在的行号 例如seg, seg-key
    std:: map<LibString, std:: map<LibString, LibString>> _segmentRefKeyValues;
    std:: map<LibString, Int32> _segmentRefMaxValidLine;                            // key：段， value:该段有效的最大行号
};

KCP_CPP_END

#endif
