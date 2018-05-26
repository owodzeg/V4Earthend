#include "Earthend.h"

using namespace std;

Earthend::Earthend()
{

}

void Earthend::getWebFileList()
{
    web_file_list = download.downloadString("pata.fufuwu.site","/storage/patafour/getfiles.php",0);
    cout << web_file_list << endl;
}

void Earthend::getLocalFileList(const char* dirn)
{
    char dirnPath[1024];
    sprintf(dirnPath, "%s\\*", dirn);

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(dirnPath, &f);

    if (h == INVALID_HANDLE_VALUE) { return; }

    do
    {
        const char * name = f.cFileName;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) { continue; }

        char filePath[1024];
        sprintf(filePath, "%s%s%s", dirn, "\\", name);

        string s_filePath = filePath;
        string filename = s_filePath.substr(s_filePath.find_last_of("\\")+1);

        if(filename.find(".") != std::string::npos)
        {
            local_files.push_back(s_filePath);
            cout << s_filePath << endl;
        }

        if (f.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
        {
            getLocalFileList(filePath);
        }

    } while (FindNextFile(h, &f));
    FindClose(h);
}

string Earthend::getFileHash()
{

}

void Earthend::downloadFile()
{

}

void Earthend::Init()
{
    if(finish == false)
    {
        ///updater code
        getWebFileList();
        getLocalFileList(".");

        finish = true;
    }
}
