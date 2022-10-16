
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_INI_FILE_DEFS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_INI_FILE_DEFS_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

class LibString;

class IniFileDefs
{
public:
    enum ContentType
    {
        Invalid = 0,            // 无效
        Segment = 1,            // 段
        KeyValue = 2,           // 键值对数据
    };

public:
    static const Byte8 _annotationFlag;        // 注释符默认:';'
    static const Byte8 _leftSegmentFlag;       // 段名左括符:'['
    static const Byte8 _rightSegmentFlag;      // 段名右括符:']'
    static const Byte8 _keyValueJoinerFlag;    // 键值对连接符:'='
    static const Byte8 _changeLineFlag;        // 换行符:'\n'
    static const Byte8 _segKeyJoinerFlag;      // 段与键连接符:'-'用于创建一个segkey的键
    static const Byte8 *_annotationSpaceStr;   // 注释间隔字符串:"\t\t\t\t"
    static const Byte8 _groupDataSeparateFlag;     // 组合数据分隔符:','
};

class IniFileMethods
{
public:
    static bool IsSegment(const LibString &content, LibString &segmentOut);
    static bool IfExistKeyValue(const LibString &content);
    static bool ExtractValidRawData(const LibString &content, Int32 &contentTypeOut, LibString &validRawDataOut); // true表示有segment或者kevalue
    static bool SpiltKeyValue(const LibString &validContent, LibString &key, LibString &value); // validContent需要剔除注释后的数据 true表示至少有key

    static bool IsEnglishChar(Byte8 ch);
    static bool IsNumChar(Byte8 ch);

    static void MakeSegKey(const LibString &segment, const LibString &key, LibString &segKeyOut);
    static void MakeKeyValuePairStr(const LibString &key, const LibString &value, LibString &keyValueStrOut);
};

KCP_CPP_END

#endif
