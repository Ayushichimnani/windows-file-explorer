#include "../include/file_details.h"
#include <sstream>
#include <iomanip>

string formatTime(FILETIME fileTime)
{
    FILETIME localTime;
    SYSTEMTIME time;

    FileTimeToLocalFileTime(&fileTime, &localTime);
    FileTimeToSystemTime(&localTime, &time);

    stringstream ss;

    ss << setfill('0')
       << setw(2) << time.wDay << "/"
       << setw(2) << time.wMonth << "/"
       << time.wYear << "  "
       << setw(2) << time.wHour << ":"
       << setw(2) << time.wMinute;

    return ss.str();
}

string getFileDetails(fs::path filePath)
{
    string details;

    details += "File Name: ";
    details += filePath.filename().string();

    details += "\r\nFile Type: ";

    if (fs::is_directory(filePath))
    {
        details += "Folder";
    }
    else
    {
        string extension = filePath.extension().string();

        if (extension.empty())
            details += "No Extension";
        else
            details += extension;
    }

    details += "\r\nFile Size: ";

    if (fs::is_regular_file(filePath))
    {
        auto size = fs::file_size(filePath);

        details += to_string(size) + " bytes";
    }
    else
    {
        details += "Not applicable";
    }

    HANDLE fileHandle = CreateFileA(
        filePath.string().c_str(),
        FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        FILETIME creationTime;
        FILETIME accessTime;
        FILETIME modifiedTime;

        if (GetFileTime(
            fileHandle,
            &creationTime,
            &accessTime,
            &modifiedTime
        ))
        {
            details += "\r\nCreated: ";
            details += formatTime(creationTime);

            details += "\r\nLast Accessed: ";
            details += formatTime(accessTime);

            details += "\r\nLast Modified: ";
            details += formatTime(modifiedTime);
        }

        CloseHandle(fileHandle);
    }
    else
    {
        details += "\r\nDate information unavailable";
    }

    return details;
}
