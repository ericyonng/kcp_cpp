

#include <pch.h>
#include <kcp/util/FileUtil.h>
#include <kcp/util/DirectoryUtil.h>
#include <kcp/util/StringUtil.h>
#include <kcp/LibTime.h>

KCP_CPP_BEGIN


const Byte8 * FileUtil::GenRandFileNameNoDir(Byte8 randName[L_tmpnam])
{
    if(tmpnam(randName))
    {
        const auto &fileName = DirectoryUtil::GetFileNameInPath(randName);
        if(fileName.GetRaw().length() > 0)
        {
            randName[0] = 0;
            auto len = sprintf(randName, "%s", fileName.c_str());
            len = ((len < L_tmpnam) ? std::max<Int32>(len, 0) : (L_tmpnam - 1));
            randName[len] = 0;
            return randName;
        }
    }

    return NULL;
}

UInt64 FileUtil::ReadUtf8OneLine(FILE &fp, LibString &outBuffer, UInt64 *utf8CharCount)
{
    // 读取单字节字符时候判断是否\n
    clearerr(&fp);
    U8 get_c = 0;
    UInt64 cnt = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        auto bytes = fread(&get_c, sizeof(get_c), 1, &fp);
        if(bytes == 1)
        {
            // 该utf8字符总字节数
            auto totalBytes = StringUtil::CalcUtf8CharBytes(get_c);
            UInt64 leftBytes = totalBytes - 1;
            if(!leftBytes)
            {// 单字节字符

                #ifdef _WIN32
                            
                    if(get_c == '\r')
                        continue;

                    if(get_c != '\n')
                    {
                        outBuffer.AppendData(reinterpret_cast<const char *>(&get_c), 1);
                        ++cnt;

                        if((utf8CharCount))
                            ++(*utf8CharCount);
                    }
                    else
                    {
                        break;
                    }
                #else
                    if(get_c != '\n')
                    {
                        outBuffer.AppendData(reinterpret_cast<const char *>(&get_c), 1);
                        ++cnt;
                        
                        if((utf8CharCount))
                            ++(*utf8CharCount);
                    }
                    else
                    {
                        //fread(&get_c, sizeof(get_c), 1, fpOutCache);
                        break;
                    }
                #endif
            }
            else
            {// 多字节字符

                outBuffer.AppendData(reinterpret_cast<const char *>(&get_c), 1);
                ++cnt;
                
                if((utf8CharCount))
                    ++(*utf8CharCount);

                // 读取剩下字节
                do
                {
                    if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
                    {
                        outBuffer.AppendData(reinterpret_cast<const char *>(&get_c), 1);
                        ++cnt;
                    }
                    else
                    {// 错误字符
                        break;
                    }
                } while (--leftBytes);

                // 错误字符
                if((leftBytes))
                {
                    break;
                }
            }
        }
        else
        {
            // 文件结束
            break;
        }
    }

    return cnt;
}

KCP_CPP_END
