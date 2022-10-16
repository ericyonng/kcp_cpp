#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_DIRECTORY_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_DIRECTORY_UTIL_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

class DirectoryUtil 
{
public:
    // create folder
    static bool CreateDir(const LibString &path);

    // get file/dir from path
    static LibString GetFileNameInPath(const LibString &path);
    static LibString GetFileNameInPath(const char *path);
    static LibString GetFileDirInPath(const LibString &path);
    static LibString GetFileDirInPath(const char *path);

private:
    // create sub dir
    static bool _CreateSubDir(const LibString &subDir);
    static bool _CreateSubDir(const std::string &subDir);
    // recursive create dir
    static bool _CreateRecursiveDir(const LibString &masterDir, const LibString &subDir);
};

inline LibString DirectoryUtil::GetFileNameInPath(const char *path)
{
    return GetFileNameInPath(LibString(path));
}

inline LibString DirectoryUtil::GetFileDirInPath(const char *path)
{
    return GetFileDirInPath(LibString(path));
}

inline bool DirectoryUtil::_CreateSubDir(const LibString &subDir)
{
    return _CreateSubDir(subDir.GetRaw());
}

KCP_CPP_END

#endif
