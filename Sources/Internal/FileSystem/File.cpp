#include "FileSystem/File.h"

#include "../Platform/TemplateAndroid/AssetsManagerAndroid.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/ResourceArchive.h"
#include "FileSystem/DynamicMemoryFile.h"
#include "FileSystem/FileAPIHelper.h"

#include "Utils/StringFormat.h"
#include "Concurrency/Mutex.h"
#include "Concurrency/LockGuard.h"
#include "Core/Core.h"
#include "PackManager/PackManager.h"

#if defined(__DAVAENGINE_WINDOWS__)
#include <io.h>
#elif defined(__DAVAENGINE_ANDROID__) || defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__)
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <time.h>

namespace DAVA
{
File::~File()
{
    // Though File object is created through Create methods returning nullptr on error
    // pointer should be checked against nullptr as class File can have inheritors
    // which do not initialize file pointer (e.g. DynamicMemoryFile)
    if (file != nullptr)
    {
        fclose(file);
        file = nullptr;
    }
}

File* File::Create(const FilePath& filePath, uint32 attributes)
{
    File* result = CreateFromSystemPath(filePath, attributes);
    return result; // easy debug on android(can set breakpoint on nullptr value in eclipse do not remove it)
}

File* File::CreateFromSystemPath(const FilePath& filename, uint32 attributes)
{
    FileSystem* fileSystem = FileSystem::Instance();

    if (FilePath::PATH_IN_RESOURCES == filename.GetType() && !((attributes & CREATE) || (attributes & WRITE)))
    {
        String relative = filename.GetRelativePathname("~res:/");

        // now with PackManager we can improve perfomance by lookup pack name
        // from DB with all files, then check if such pack mounted and from
        // mountedPackIndex find by name archive with file or skip to next step
        PackManager& pm = Core::Instance()->GetPackManager();
        Vector<uint8> contentAndSize;

        if (pm.IsGpuPacksInitialized())
        {
            const String& packName = pm.FindPackName(filename);
            if (!packName.empty())
            {
                auto it = fileSystem->resArchiveMap.find(packName);
                if (it != end(fileSystem->resArchiveMap))
                {
                    if (it->second.archive->LoadFile(relative, contentAndSize))
                    {
                        return DynamicMemoryFile::Create(std::move(contentAndSize), READ, filename);
                    }
                }
            }
        }

        for (auto& pair : fileSystem->resArchiveMap)
        {
            if (pair.second.archive->LoadFile(relative, contentAndSize))
            {
                return DynamicMemoryFile::Create(std::move(contentAndSize), READ, filename);
            }
        }
    }

    return PureCreate(filename, attributes);
}

static File* CreateFromAPK(const FilePath& filePath, uint32 attributes)
{
    static Mutex mutex;

    LockGuard<Mutex> guard(mutex);

    AssetsManagerAndroid* assetsManager = AssetsManagerAndroid::Instance();
    DVASSERT_MSG(assetsManager, "[CreateFromAPK] Need to create AssetsManager before loading files");

    Vector<uint8> data;
    if (!assetsManager->LoadFile(filePath.GetAbsolutePathname(), data))
    {
        return nullptr;
    }

    return DynamicMemoryFile::Create(std::move(data), attributes, filePath);
}

File* File::PureCreate(const FilePath& filePath, uint32 attributes)
{
    FILE* file = 0;
    uint32 size = 0;
    FilePath::NativeStringType path = filePath.GetNativeAbsolutePathname();

    if ((attributes & File::OPEN) && (attributes & File::READ))
    {
        if (attributes & File::WRITE)
        {
            file = FileAPI::OpenFile(path.c_str(), NativeStringLiteral("r+b"));
        }
        else
        {
            file = FileAPI::OpenFile(path.c_str(), NativeStringLiteral("rb"));
        }

        if (!file)
        {
#ifdef __DAVAENGINE_ANDROID__
            File* fromAPK = CreateFromAPK(filePath, attributes);
            return fromAPK; // simpler debugging on android
#else
            return nullptr;
#endif
        }
        fseek(file, 0, SEEK_END);
        size = static_cast<uint32>(ftell(file));
        fseek(file, 0, SEEK_SET);
    }
    else if ((attributes & File::CREATE) && (attributes & File::WRITE))
    {
        file = FileAPI::OpenFile(path.c_str(), NativeStringLiteral("wb"));
        if (!file)
            return nullptr;
    }
    else if ((attributes & File::APPEND) && (attributes & File::WRITE))
    {
        file = FileAPI::OpenFile(path.c_str(), NativeStringLiteral("ab"));
        if (!file)
            return nullptr;
        fseek(file, 0, SEEK_END);
        size = static_cast<uint32>(ftell(file));
    }
    else
    {
        return nullptr;
    }

    File* fileInstance = new File();
    fileInstance->filename = filePath;
    fileInstance->size = size;
    fileInstance->file = file;
    return fileInstance;
}

const FilePath& File::GetFilename()
{
    return filename;
}

uint32 File::Write(const void* pointerToData, uint32 dataSize)
{
#if defined(__DAVAENGINE_ANDROID__)
    uint32 posBeforeWrite = GetPos();
#endif

    //! Do not change order fread return not bytes -- items
    uint32 lSize = static_cast<uint32>(fwrite(pointerToData, 1, dataSize, file));

#if defined(__DAVAENGINE_ANDROID__)
    //for Android value returned by 'fwrite()' is incorrect in case of full disk, that's why we calculate 'lSize' using 'GetPos()'
    lSize = GetPos() - posBeforeWrite;
#endif

    size += lSize;

    return lSize;
}

uint32 File::Read(void* pointerToData, uint32 dataSize)
{
    //! Do not change order (1, dataSize), cause fread return count of size(2nd param) items
    //! May be performance issues
    return static_cast<uint32>(fread(pointerToData, 1, static_cast<size_t>(dataSize), file));
}

uint32 File::ReadString(char8* destinationBuffer, uint32 destinationBufferSize)
{
    uint32 writeIndex = 0;
    uint8 currentChar = 0;

    if (destinationBufferSize > 0)
    {
        while (Read(&currentChar, 1) > 0)
        {
            if (writeIndex < destinationBufferSize)
            {
                destinationBuffer[writeIndex] = currentChar;
                writeIndex++;
            }
            else
            {
                currentChar = 0;
                Logger::Warning("File::ReadString buffer size is too small for this string.");
            }

            if (currentChar == 0)
            {
                writeIndex--;
                break;
            }
        }

        destinationBuffer[writeIndex] = 0;
    }

    return writeIndex;
}

uint32 File::ReadString(String& destinationString)
{
    uint32 writeIndex = 0;
    uint8 currentChar = 0;

    while (!IsEof() && Read(&currentChar, 1) != 0)
    {
        if (0 != currentChar)
        {
            destinationString += currentChar;
            writeIndex++;
        }
        else
        {
            break;
        }
    }
    return writeIndex - 1;
}

uint32 File::ReadLine(void* pointerToData, uint32 bufferSize)
{
    uint32 ret = 0;

    if (bufferSize > 0)
    {
        uint8* inPtr = reinterpret_cast<uint8*>(pointerToData);
        while (!IsEof() && bufferSize > 1)
        {
            uint8 nextChar;
            if (GetNextChar(&nextChar))
            {
                *inPtr = nextChar;
                inPtr++;
                bufferSize--;
            }
            else
            {
                break;
            }
        }
        *inPtr = 0;
        inPtr++;
        ret = static_cast<uint32>(inPtr - reinterpret_cast<uint8*>(pointerToData));
    }

    return ret;
}

String File::ReadLine()
{
    String destinationString;
    while (!IsEof())
    {
        uint8 nextChar;
        if (GetNextChar(&nextChar))
        {
            destinationString += nextChar;
        }
        else
        {
            break;
        }
    }
    return destinationString;
}

bool File::GetNextChar(uint8* nextChar)
{
    uint64 actuallyRead = Read(nextChar, 1);
    if (actuallyRead != 1)
    {
        //seems IsEof()
        return false;
    }

    if (0 == *nextChar)
    {
        // 0 terminated string
        return false;
    }
    else if ('\r' == *nextChar)
    {
        // we don't need to return \r as a charracter
        return GetNextChar(nextChar);
    }
    else if ('\n' == *nextChar)
    {
        // there was a last charracter in string ended by \n, then we cannot read more
        return false;
    }
    else
    {
        // some regular charracter readed
        return true;
    }
}

uint64 File::GetPos() const
{
#if defined(__DAVAENGINE_WINDOWS__)
    return static_cast<uint32>(_ftelli64(file));
#else
    return static_cast<uint64>(ftello(file));
#endif
}

uint64 File::GetSize() const
{
    return size;
}

bool File::Seek(int64 position, eFileSeek seekType)
{
    int realSeekType = 0;
    switch (seekType)
    {
    case SEEK_FROM_START:
        realSeekType = SEEK_SET;
        break;
    case SEEK_FROM_CURRENT:
        realSeekType = SEEK_CUR;
        break;
    case SEEK_FROM_END:
        realSeekType = SEEK_END;
        break;
    default:
        DVASSERT(0 && "Invalid seek type");
        break;
    }

#if defined(__DAVAENGINE_WINDOWS__)
    return 0 == _fseeki64(file, position, realSeekType);
#else
    return 0 == fseeko(file, position, realSeekType);
#endif
}

bool File::Flush()
{
    return 0 == fflush(file);
}

bool File::IsEof() const
{
    return (feof(file) != 0);
}

bool File::Truncate(uint64 size)
{
#if defined(__DAVAENGINE_WINDOWS__)
    return (0 == _chsize(_fileno(file), static_cast<long>(size)));
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_ANDROID__)
    return (0 == ftruncate(fileno(file), size));
#else
#error No implementation for current platform
    return false;
#endif
}

bool File::WriteString(const String& strtowrite, bool shouldNullBeWritten)
{
    const char* str = strtowrite.c_str();
    uint32 null = (shouldNullBeWritten) ? (1) : (0);
    return (Write(str, static_cast<uint32>(strtowrite.length() + null)) == strtowrite.length() + null);
}

bool File::WriteNonTerminatedString(const String& strtowrite)
{
    const char* str = strtowrite.c_str();
    return (Write(str, static_cast<uint32>(strtowrite.length())) == strtowrite.length());
}

bool File::WriteLine(const String& string)
{
    uint32 written = 0;
    const char* str = string.c_str();
    const char* endLine = "\r\n";
    uint32 endLength = static_cast<uint32>(strlen(endLine));
    uint32 strLength = static_cast<uint32>(string.length());

    written += Write(str, strLength);
    written += Write(endLine, endLength);

    return (written == strLength + endLength);
}

String File::GetModificationDate(const FilePath& filePathname)
{
    String realPathname = filePathname.GetAbsolutePathname();

    struct stat fileInfo = { 0 };
    int32 ret = stat(realPathname.c_str(), &fileInfo);
    if (0 == ret)
    {
#if defined(__DAVAENGINE_WINDOWS__)
        tm* utcTime = gmtime(&fileInfo.st_mtime);
#elif defined(__DAVAENGINE_ANDROID__)
        tm* utcTime = gmtime((const time_t*)&fileInfo.st_mtime);
#elif defined(__DAVAENGINE_MACOS__) || defined(__DAVAENGINE_IPHONE__)
        tm* utcTime = gmtime(&fileInfo.st_mtimespec.tv_sec);
#endif
        return String(Format("%04d.%02d.%02d %02d:%02d:%02d",
                             utcTime->tm_year + 1900, utcTime->tm_mon + 1, utcTime->tm_mday,
                             utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec));
    }
    return String("");
}
}
