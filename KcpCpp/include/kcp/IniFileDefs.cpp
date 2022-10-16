
#include <pch.h>
#include <kcp/IniFileDefs.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN


const Byte8 IniFileDefs::_annotationFlag = ';';
const Byte8 IniFileDefs::_leftSegmentFlag = '[';
const Byte8 IniFileDefs::_rightSegmentFlag = ']';
const Byte8 IniFileDefs::_keyValueJoinerFlag = '=';
const Byte8 IniFileDefs::_changeLineFlag = '\n';
const Byte8 IniFileDefs::_segKeyJoinerFlag = '-';
const Byte8 *IniFileDefs::_annotationSpaceStr = "\t\t\t\t";
const Byte8 IniFileDefs::_groupDataSeparateFlag = ',';

bool IniFileMethods::IsSegment(const LibString &content, LibString &segmentOut)
{
    // 包含[] 且[]中间的字符去除左右空后是连续的英文或数值
    auto cache = content;
    cache.lstrip();
    cache.rstrip();

    auto &cacheRaw = cache.GetRaw();
    auto leftBracketsPos = cacheRaw.find(IniFileDefs::_leftSegmentFlag, 0);
    if(leftBracketsPos == std::string::npos)
        return false;

    auto rightBracketsPos = cacheRaw.find(IniFileDefs::_rightSegmentFlag, leftBracketsPos);
    if(rightBracketsPos == std::string::npos)
        return false;

    // 判断有没有中括号间是否有字符
    if(rightBracketsPos - leftBracketsPos - 1 == 0)
        return false;

    // 提取粗的segment
    LibString segmentTmp = cacheRaw.substr(leftBracketsPos + 1, rightBracketsPos - leftBracketsPos - 1);
    segmentTmp.lstrip();
    segmentTmp.rstrip();

    // 是否连续的英文或者数值，且首字符为英文
    if(!IsEnglishChar(segmentTmp[0]))
        return false;

    const auto len = segmentTmp.size();
    auto &segmentRaw = segmentTmp.GetRaw();
    for(size_t i = 0; i < len; ++i)
    {
        if(!IsEnglishChar(segmentRaw.at(i)) &&
           !IsNumChar(segmentRaw.at(i)))
            return false;
    }

    segmentOut = segmentRaw;
    return true;
}

bool IniFileMethods::IfExistKeyValue(const LibString &content)
{
    auto &raw = content.GetRaw();
    return raw.find(IniFileDefs::_keyValueJoinerFlag, 0) != std::string::npos;
}

bool IniFileMethods::ExtractValidRawData(const LibString &content, Int32 &contentTypeOut, LibString &validRawDataOut)
{
    contentTypeOut = IniFileDefs::Invalid;
    if(content.empty())
        return false;

    // 分离注释
    auto pieces = content.Split(IniFileDefs::_annotationFlag, 1, true);
    if(pieces.empty())
        return false;
    auto rawData = pieces[0];
    if(rawData.empty())
        return false;

    // 去除无效符号
    rawData.lstrip();
    rawData.rstrip();

    // 判断是否有效数据 键值对或者segment
    if(IsSegment(rawData, validRawDataOut))
    {
        contentTypeOut = IniFileDefs::Segment;
        return true;
    }

    if(IfExistKeyValue(rawData))
    {
        validRawDataOut = rawData;
        contentTypeOut = IniFileDefs::KeyValue;
        return true;
    }

    return false;
}

bool IniFileMethods::SpiltKeyValue(const LibString &validContent, LibString &key, LibString &value)
{
    if(validContent.empty())
        return false;

    // 包含=号，且=左边key值在去除无效符号后第一个为英文其他是连续的英文或者数值
    auto &raw = validContent.GetRaw();
    auto eqPos = raw.find_first_of(IniFileDefs::_keyValueJoinerFlag, 0);
    if(eqPos == std::string::npos)
        return false;

    // 获得key的粗数据
    LibString keyRaw = raw.substr(0, eqPos);
    if(keyRaw.empty())
        return false;

    {// 消除key的无效值第一个英文字符开始
        keyRaw.lstrip();
        keyRaw.rstrip();
        if(!IniFileMethods::IsEnglishChar(keyRaw[0]))
            return false;

        // key必须是连续的英文或者数值
        auto &keyRawRaw = keyRaw.GetRaw();
        const auto len = keyRawRaw.length();
        for(size_t i = 0; i < len; ++i)
        {
            if(!IniFileMethods::IsEnglishChar(keyRawRaw.at(i)) &&
               !IniFileMethods::IsNumChar(keyRawRaw.at(i)))
                return false;
        }

        key = keyRawRaw.substr(0, keyRawRaw.size());
    }

    if(key.empty())
        return false;

    // 可能会没有value值
    if(raw.size() <= eqPos + 1)
        return !key.empty();

    // 获得value的粗数据 可能会为空
    LibString valueRaw = raw.substr(eqPos + 1, raw.size() - eqPos - 1);
    if(valueRaw.empty())
        return true;

    {// 去除左右的无效符号即为所要的value
        valueRaw.lstrip();
        valueRaw.rstrip();
        value = valueRaw;
    }

    return true;
}

bool IniFileMethods::IsEnglishChar(char ch)
{
    return ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z';
}

bool IniFileMethods::IsNumChar(char ch)
{
    return ch >= '0'&&ch <= '9';
}

void IniFileMethods::MakeSegKey(const LibString &segment, const LibString &key, LibString &segKeyOut)
{
    segKeyOut = segment + LibString(IniFileDefs::_segKeyJoinerFlag) + key;
}

void IniFileMethods::MakeKeyValuePairStr(const LibString &key, const LibString &value, LibString &keyValueStrOut)
{
    keyValueStrOut = key + LibString(IniFileDefs::_keyValueJoinerFlag) + value;
}

KCP_CPP_END
