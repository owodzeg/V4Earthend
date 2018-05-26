#ifndef EARTHEND_H
#define EARTHEND_H

#include "Download.h"
#include <iostream>
#include <windows.h>

using namespace std;

class Earthend
{
    public:
    Download download;
    string web_file_list;

    vector<string> web_files;
    vector<string> local_files;

    bool finish = false;

    Earthend();

    void getWebFileList();
    void getLocalFileList(const char* dirn);
    string getFileHash();
    void downloadFile();

    void Init();
};

#endif // EARTHEND_H
