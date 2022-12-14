

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_FILE_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_FILE_UTIL_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>
#include <kcp/LibTime.h>

KCP_CPP_BEGIN

class LibTime;

class FileUtil
{
public:
    static void DelFile(const Byte8 *filePath);
    static bool DelFileCStyle(const Byte8 *filePath);
    static FILE *CreateTmpFile();
    static const Byte8 *GenRandFileName(Byte8 randName[L_tmpnam]);
    static const Byte8 *GenRandFileNameNoDir(Byte8 randName[L_tmpnam]);
    // 清除文件错误 不是追加打开文件的，文件光标设置开头
    static FILE *OpenFile(const Byte8 *fileName, bool isCreate = false, const Byte8 *openType = "rb+");
    static bool CopyFile(const Byte8 *srcFile, const Byte8 *destFile);
    static bool CopyFile(FILE &src, FILE &dest);

    static UInt64 ReadFile(FILE &fp, UInt64 bufferSize, Byte8 *&buffer);
    static UInt64 ReadFile(FILE &fp, LibString &outString, Int64 sizeLimit = -1);
    static Int64 WriteFile(FILE &fp, const Byte8 *buffer, Int64 dataLenToWrite);
    static Int64 WriteFile(FILE &fp, const LibString &bitData);
    static bool IsEnd(FILE &fp);
    static bool CloseFile(FILE &fp);
    static bool IsFileExist(const Byte8 *fileName);
    static Int32 GetFileCusorPos(FILE &fp);
    // enum pos:FileCursorOffsetType::FILE_CURSOR_POS
    static bool SetFileCursor(FILE &fp, Int32 enumPos, Long offset);
    static void ResetFileCursor(FILE &fp);
    static bool FlushFile(FILE &fp);
    static Long GetFileSize(FILE &fp);
    static Int64 GetFileSizeEx(const Byte8 *filepath);
    static void InsertFileTime(const LibString &extensionName, const LibTime &timestamp, LibString &fileName);
    static void InsertFileTail(const LibString &extensionName, const Byte8 *tail, LibString &fileName);
    static LibString ExtractFileExtension(const LibString &fileName);
    static LibString ExtractFileWithoutExtension(const LibString &fileName); 
    static Int32 GetFileNo(FILE *fp);

    #pragma region
    // 读取ASCII行
    static UInt64 ReadOneLine(FILE &fp, UInt64 bufferSize, Byte8 *&buffer);  // 不包含\n
    static UInt64 ReadOneLine(FILE &fp, LibString &outBuffer);              // 不包含\n

    // 读取utf8行
    static UInt64 ReadUtf8OneLine(FILE &fp, LibString &outBuffer, UInt64 *utf8CharCount = NULL); // 不包含\n(剔除\n)
    #pragma endregion
};

inline void FileUtil::DelFile(const Byte8 *filePath)
{
#ifdef _WIN32
    std::string strDelCmd = "del ";
    strDelCmd += filePath;
    size_t findPos = 0;
    int nCount = 0;
    const auto strCount = strDelCmd.length();
    while((findPos = strDelCmd.find_first_of('/', findPos)) != std::string::npos)
    {
        strDelCmd[findPos] = '\\';
    }
    strDelCmd += " /f/s/q";

    system(strDelCmd.c_str());
#else
    std::string strDelCmd = "sudo rm -rf ";
    strDelCmd += filePath;
    if(system(strDelCmd.c_str()) == -1)
        perror("del files fail");
#endif
}

inline bool FileUtil::DelFileCStyle(const Byte8 *filePath)
{
    return remove(filePath) == 0;
}

inline FILE  *FileUtil::CreateTmpFile()
{
    return tmpfile();
}

inline const Byte8 *FileUtil::GenRandFileName(Byte8 randName[L_tmpnam])
{
    return tmpnam(randName);
}

inline FILE *FileUtil::OpenFile(const Byte8 *fileName, bool isCreate /*= false*/, const Byte8 *openType /*= "rb+"*/)
{
    if(fileName == NULL || openType == NULL)
        return NULL;

    FILE *fp = NULL;
    fp = ::fopen(fileName, openType);
    if(!fp)
    {
        if(isCreate)
        {
            fp = fopen(fileName, "wb+");
            if(!fp)
                return NULL;
        }
        else
        {
            return NULL;
        }
    }

    clearerr(fp);

    // 不是追加打开文件的设置开头
    if(!strchr(openType, 'a'))
        rewind(fp);
    return fp;
}

inline bool FileUtil::CopyFile(const Byte8 *srcFile, const Byte8 *destFile)
{
    if((!srcFile || !destFile))
        return false;

    auto srcFp = OpenFile(srcFile);
    if(!srcFp)
        return false;

    auto destFp = OpenFile(destFile, true, "wb+");
    if(!destFp)
        return false;

    unsigned char get_c = 0;
    char count = 0, wrCount = 0;
    bool isDirty = false;

    while(!feof(srcFp))
    {
        get_c = 0;
        count = char(fread(&get_c, 1, 1, srcFp));
        if(count != 1)
            break;

        wrCount = char(fwrite(&get_c, 1, 1, destFp));
        if(wrCount != 1)
            break;

        isDirty = true;
    }

    if(isDirty)
        FlushFile(*destFp);

    CloseFile(*srcFp);
    CloseFile(*destFp);
    return true;
}

inline bool FileUtil::CopyFile(FILE &src, FILE &dest)
{
    clearerr(&src);
    clearerr(&dest);
    unsigned char get_c = 0;
    char count = 0, wrCount = 0;
    bool isDirty = false;
    while(!feof(&src))
    {
        get_c = 0;
        count = char(fread(&get_c, 1, 1, &src));
        if(count != 1)
            break;

        wrCount = char(fwrite(&get_c, 1, 1, &dest));
        if(wrCount != 1)
            break;

        isDirty = true;
    }

    if(isDirty)
        FlushFile(dest);

    return true;
}

inline UInt64 FileUtil::ReadFile(FILE &fp, UInt64 bufferSize, Byte8 *&buffer)
{
    if(!buffer || !bufferSize)
        return 0;

    clearerr(&fp);
    UInt64 readCnt = 0;
    U8 *bufferTmp = reinterpret_cast<U8 *>(buffer);
    U8 get_c = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
            *bufferTmp = get_c;
            ++bufferTmp;
            ++readCnt;

            if(readCnt >= bufferSize)
                break;
        }
        else
        {
            break;
        }
    }

    return readCnt;
}

inline UInt64 FileUtil::ReadFile(FILE &fp, LibString &outString, Int64 sizeLimit)
{
    clearerr(&fp);
    UInt64 readCnt = 0;
    U8 get_c = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
            outString.AppendData(reinterpret_cast<const Byte8 *>(&get_c), 1);
            ++readCnt;

            if(sizeLimit > 0 && 
               static_cast<Int64>(readCnt) >= sizeLimit)
                break;
        }
        else
        {
            break;
        }
    }

    return readCnt;
}

inline Int64 FileUtil::WriteFile(FILE &fp, const Byte8 *buffer, Int64 dataLenToWrite)
{
//     if(!buffer || dataLenToWrite == 0)
//         return 0;

    clearerr(&fp);
    Int64 idx = 0;
    while(dataLenToWrite != 0)
    {
        if(LIKELY(fwrite(buffer + idx, 1, 1, &fp) == 1))
        {
            --dataLenToWrite;
            ++idx;
        }
        else
        {
            break;
        }
    }

//     if(dataLenToWrite != cnt)
//         printf("write error!");

    return idx;
}

inline Int64 FileUtil::WriteFile(FILE &fp, const LibString &bitData)
{
    return WriteFile(fp, bitData.GetRaw().data(), bitData.size());
}

inline bool FileUtil::IsEnd(FILE &fp)
{
    return feof(&fp);
}

inline bool FileUtil::CloseFile(FILE &fp)
{
    clearerr(&fp);
    if(fclose(&fp) != 0)
        return false;

    return true;
}

inline bool FileUtil::IsFileExist(const Byte8 *fileName)
{
    if((!fileName))
        return false;

#ifdef _WIN32
    if(::_access(fileName, 0) == -1)
        return false;
#else
    if(::access(fileName, 0) == -1)
        return false;
#endif

    return true;
}

inline Int32 FileUtil::GetFileCusorPos(FILE &fp)
{
    return ftell(&fp);
}

inline bool FileUtil::SetFileCursor(FILE &fp, Int32 enumPos, Long offset)
{
    return fseek(&fp, offset, enumPos) == 0;
}

inline void FileUtil::ResetFileCursor(FILE &fp)
{
    clearerr(&fp);
    rewind(&fp);
}

inline bool FileUtil::FlushFile(FILE &fp)
{
    return fflush(&fp) == 0;
}

inline Long FileUtil::GetFileSize(FILE &fp)
{
    auto curPos = ftell(&fp);
    if(curPos < 0)
        return -1;

    if((!SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_END, 0L)))
        return -1;

    auto fileSize = ftell(&fp);
    if((fileSize < 0))
    {
        SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_SET, curPos);
        return -1;
    }

    SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_SET, curPos);
    return fileSize;
}

inline Int64 FileUtil::GetFileSizeEx(const Byte8 *filepath)
{
#ifdef _WIN32
    struct _stat info;
    if(::_stat(filepath, &info) != 0)
        return -1;

    return info.st_size;
#else
    struct stat info;
    if(::stat(filepath, &info) != 0)
        return -1;

    return info.st_size;
#endif

}

inline void FileUtil::InsertFileTime(const LibString &extensionName, const LibTime &timestamp, LibString &fileName)
{
    std::string &raw = fileName.GetRaw();
    auto endPos = raw.rfind('.', fileName.length() - 1);
    const auto &timeFmtStr = timestamp.Format("-%Y-%m-%d");
    if(endPos == std::string::npos)
    {
        fileName << timeFmtStr << extensionName;
        return;
    }
    raw.insert(endPos, timeFmtStr.GetRaw());
}

inline void FileUtil::InsertFileTail(const LibString &extensionName, const Byte8 *tail, LibString &fileName)
{
    std::string &raw = fileName.GetRaw();
    auto endPos = raw.rfind('.', fileName.length() - 1);
    if(endPos == std::string::npos)
    {
        fileName << tail << extensionName;
        return;
    }

    raw.insert(endPos, tail);
}

inline LibString FileUtil::ExtractFileExtension(const LibString &fileName)
{
    const std::string &raw = fileName.GetRaw();
    auto endPos = raw.rfind('.', fileName.length() - 1);
    if(endPos == std::string::npos)
        return "";

    return raw.substr(endPos, fileName.length() - endPos);
}

inline LibString FileUtil::ExtractFileWithoutExtension(const LibString &fileName)
{
    const std::string &raw = fileName.GetRaw();
    auto endPos = raw.rfind('.', fileName.length() - 1);
    if(endPos == std::string::npos)
        return fileName;

    if(endPos == 0)
        return "";

    return raw.substr(0, endPos);
}

inline Int32 FileUtil::GetFileNo(FILE *fp)
{
#ifdef _WIN32
    int fileNo = ::_fileno(fp);
#else
    int fileNo = ::fileno(fp);
#endif

    if ((fileNo == -1))
    {
        return -1;
    }

    return fileNo;
}

inline UInt64 FileUtil::ReadOneLine(FILE &fp, UInt64 bufferSize, Byte8 *&buffer)
{
    if(!buffer || bufferSize == 0)
        return 0;

    U8 get_c = 0;
    U8 *bufferTmp = reinterpret_cast<U8 *>(buffer);
    ::memset(bufferTmp, 0, bufferSize);

    clearerr(&fp);
    UInt64 cnt = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
#ifdef _WIN32
            if(get_c == '\r')
                continue;

            if(get_c != '\n')
            {
                *bufferTmp = get_c;
                ++bufferTmp;
                ++cnt;

                if(bufferSize <= cnt) 
                    break;
            }
            else
            {
                //SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_CUR, 0);
                break;
            }

#else
            if(get_c != '\n')
            {
                *bufferTmp++ = get_c;
                ++cnt;

                if(bufferSize <= cnt) 
                    break;
            }
            else
            {
                //fread(&get_c, sizeof(get_c), 1, fpOutCache);
                break;
            }
#endif

        }
        else
        {
            break;
        }
    }

    return cnt;
}

inline UInt64 FileUtil::ReadOneLine(FILE &fp, LibString &outBuffer)
{
    clearerr(&fp);
    unsigned char get_c = 0;
    UInt64 cnt = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
#ifdef _WIN32
            if(get_c == '\r')
                continue;

            if(get_c != '\n')
            {
                outBuffer.AppendData(reinterpret_cast<const char *>(&get_c), 1);
                ++cnt;
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
            }
            else
            {
                //fread(&get_c, sizeof(get_c), 1, fpOutCache);
                break;
            }
#endif
        }
        else
        {
            break;
        }
    }

    return cnt;
}

KCP_CPP_END

#endif
