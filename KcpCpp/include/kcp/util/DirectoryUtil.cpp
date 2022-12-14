
#include <pch.h>
#include <kcp/util/DirectoryUtil.h>

KCP_CPP_BEGIN

bool DirectoryUtil::CreateDir(const LibString &path)
{
    if(path.empty())
        return false;

    LibString rootDir, subDir;
    auto &pathRaw = path.GetRaw();
#ifdef _WIN32
    auto startPos = path.GetRaw().find(':', 0);    // judge if include path
    if(startPos == std::string::npos)
    {
        // 没有盘符
        rootDir = "";
        subDir << "\\";
        subDir << pathRaw;
    }
    else
    {
        // 截取盘符
        rootDir = pathRaw.substr(startPos - 1, 1);
        rootDir << ":\\";

        // 盘符之后的子目录
        startPos = pathRaw.find("\\", startPos + 1);
        if(startPos == std::string::npos)
            return false;

        // 子目录路径
        subDir = pathRaw.substr(startPos, path.length() - startPos);
    }
#else
    if(pathRaw.at(0) == '/')
    {
        rootDir = "/";
        subDir = path;
    }
    else
    {
        auto startPos = pathRaw.find_first_of('/', 0);
        if(startPos == std::string::npos)
            return false;

        subDir = pathRaw.substr(startPos, path.length() - startPos);
        rootDir = pathRaw.substr(0, startPos);
    }
#endif

    return _CreateRecursiveDir(rootDir, subDir);
}

LibString DirectoryUtil::GetFileNameInPath(const LibString &path)
{
    if(!(path.GetRaw().length() > 0))
        return "";

    auto &pathRaw = path.GetRaw();
    Byte8 c = 0;
    Int32 i = 0;
    const Int32 len = static_cast<Int32>(pathRaw.length());
    for(i = len - 1; i >= 0; --i)
    {
        c = pathRaw.at(i);
        if(c == '\\' || c == '/')
        {
            ++i;
            break;
        }
    }

    if((i < 0))
        return "";


    return pathRaw.substr(i, len - i);
}

LibString DirectoryUtil::GetFileDirInPath(const LibString &path)
{
    if(path.GetRaw().length() <= 0)
        return "";

    char c = 0;
    int i = 0;
    auto &pathRaw = path.GetRaw();
    const Int32 len = static_cast<Int32>(pathRaw.length());
    for(i = len - 1; i >= 0; --i)
    {
        c = pathRaw.at(i);
        if(c == '\\' || c == '/')
        {
            ++i;
            break;
        }
    }

    if(i < 0)
        return "";

    return pathRaw.substr(0, i);
}

bool DirectoryUtil::_CreateRecursiveDir(const LibString &masterDir, const LibString &subDir)
{
    std::string dir = "";
    std::string strtocreate = "";
    std::string strMasterPath = masterDir.GetRaw();
    int startPos = 0;
    int revStartPos = 0;
    int endPos = 0;
    int revEndPos = 0;

    int finalStartPos = 0, finalCount = 0;
    const int spritLen = static_cast<int>(strlen("\\"));
    const int revSpritLen = static_cast<int>(strlen("/"));
    const auto &subDirRaw = subDir.GetRaw();
    while(true)
    {
        startPos = static_cast<int>(subDirRaw.find("\\", startPos));
        revStartPos = static_cast<int>(subDirRaw.find("/", revStartPos));
        if(startPos == std::string::npos && 
           revStartPos == std::string::npos) 
            break;

        if(startPos == std::string::npos && revStartPos != std::string::npos)
        {
            revEndPos = static_cast<int>(subDirRaw.find("/", revStartPos + revSpritLen));
            if(revEndPos == std::string::npos) 
                break;

            finalStartPos = revStartPos + revSpritLen;
            finalCount = revEndPos - revStartPos - revSpritLen;
            revStartPos += revSpritLen;
        }
        else if(startPos != std::string::npos && revStartPos == std::string::npos)
        {
            endPos = static_cast<int>(subDirRaw.find("\\", startPos + spritLen));
            if(endPos == std::string::npos)
                break;

            finalStartPos = startPos + spritLen;
            finalCount = endPos - startPos - spritLen;
            startPos += spritLen;
        }
        else if(startPos < revStartPos)
        {
            //"\\"
            endPos = static_cast<int>(subDirRaw.find("\\", startPos + spritLen));
            revEndPos = static_cast<int>(subDirRaw.find("/", startPos + spritLen));
            if(endPos == std::string::npos && 
               revEndPos == std::string::npos) 
                break;

            if(endPos != std::string::npos && revEndPos == std::string::npos)
            {
                finalStartPos = startPos + spritLen;
                finalCount = endPos - startPos - spritLen;
            }
            else if(endPos == std::string::npos && revEndPos != std::string::npos)
            {
                finalStartPos = startPos + spritLen;
                finalCount = revEndPos - startPos - spritLen;
            }
            else if(endPos < revEndPos)
            {
                //"\\"
                finalStartPos = startPos + spritLen;
                finalCount = endPos - startPos - spritLen;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }
            else
            {
                //"/"
                finalStartPos = startPos + spritLen;
                finalCount = revEndPos - startPos - spritLen;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }

            startPos += spritLen;
        }
        else
        {
            //"/"
            finalStartPos = revStartPos + revSpritLen;
            endPos = int(subDirRaw.find("\\", revStartPos + revSpritLen));
            revEndPos = int(subDirRaw.find("/", revStartPos + revSpritLen));
            if(endPos == std::string::npos &&
               revEndPos == std::string::npos) 
                break;

            if(endPos != std::string::npos && revEndPos == std::string::npos)
            {
                finalCount = endPos - finalStartPos;
            }
            else if(endPos == std::string::npos && revEndPos != std::string::npos)
            {
                finalCount = revEndPos - finalStartPos;
            }
            else if(endPos < revEndPos)
            {
                //"\\"
                finalCount = endPos - finalStartPos;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }
            else
            {
                //"/"
                finalCount = revEndPos - finalStartPos;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }

            revStartPos += revSpritLen;
        }

        strtocreate.clear();
        dir = strMasterPath;
        if(dir.length() != 0) 
            dir += "/";

        dir += subDirRaw.substr(finalStartPos, finalCount);
        strtocreate = dir;
        strtocreate += "/";
        // CRYSTAL_TRACE("WILL CREATE strtocreate %s", strtocreate.c_str());
        if(strcmp((char *)strtocreate.c_str(), "./") != 0 && 
           strcmp((char *)strtocreate.c_str(), ".\\") != 0)
        {
            _CreateSubDir(strtocreate);
        }

        strMasterPath.clear();
        strMasterPath = dir;
    }

    return true;
}

bool DirectoryUtil::_CreateSubDir(const std::string &subDir)
{
    if(subDir.length() <= 0)
        return false;

#ifdef _WIN32    
    if(::_mkdir(subDir.c_str()) != 0)
    {
        if(::_access(subDir.c_str(), 0) == -1)
            return false;
    }
#else
    // CRYSTAL_TRACE("create sub dir %s", subDir.c_str());
    if(mkdir(subDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
    {
        if((access(subDir.c_str(), 0) == -1))
        {
            CRYSTAL_TRACE("create sub dir fail %s", subDir.c_str());
            return false;
        }
    }
#endif

    // CRYSTAL_TRACE("create dir suc %s", subDir.c_str());

    return true;
}


KCP_CPP_END
