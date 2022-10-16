

#include <pch.h>
#include <kcp/LibLogFile.h>
#include <kcp/util/FileUtil.h>

KCP_CPP_BEGIN

LibLogFile::LibLogFile()
    :_partNo(0)
{
    
}

LibLogFile::~LibLogFile()
{

}

void LibLogFile::PartitionFile(bool isSysFirstCreate, LibTime *nowTime)
{
    if(isSysFirstCreate)
        return;

    // 构建文件名
    LibString fileNameCache;
    GetCurrentFileName(fileNameCache);

    // 查找不存在的文件名
    LibString wholeName;
    wholeName.AppendFormat("%sOld%d", fileNameCache.c_str(), ++_partNo);
    while(FileUtil::IsFileExist(wholeName.c_str()))
    {
        wholeName.clear();
        wholeName.AppendFormat("%sOld%d", fileNameCache.c_str(), ++_partNo);
    }

    // 转储文件
    auto dest = FileUtil::OpenFile(wholeName.c_str(), true);
    FileUtil::ResetFileCursor(*_fp);
    FileUtil::CopyFile(*_fp, *dest);
    FileUtil::CloseFile(*dest);

    // 删除并重开文件
    Close();
    FileUtil::DelFile(fileNameCache.c_str());
    Reopen(nowTime);
}

KCP_CPP_END