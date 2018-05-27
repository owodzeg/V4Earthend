#ifndef EARTHEND_H
#define EARTHEND_H

#include "Download.h"
#include "MD5.h"
#include <iostream>
#include <windows.h>
#include <sstream>

using namespace std;

class Earthend
{
    public:
    Download download;
    string web_file_list;

    vector<string> web_files; ///web file location
    vector<string> web_hashes; ///web file hash
    vector<string> web_sizes; ///web file size

    vector<string> local_files; ///local file location

    vector<string> files_download; ///file to download
    vector<string> files_remove; ///files to remove

    bool finish = false;

    Earthend();

    void getWebFileList();
    void getLocalFileList(const char* dirn);
    string getFileHash(string filename);
    vector<string> split(const std::string &s, char delim);
    void splitWebFileList();
    void verifyFiles();
    void downloadFile();

    void Init();
};

#endif // EARTHEND_H
