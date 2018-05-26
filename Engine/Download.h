#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <SFML/Network.hpp>
#include <fstream>

using namespace std;

class Download
{
    public:
    Download();

    void downloadFile(const std::string& Host, const std::string& RemoteFile, const std::string& LocalFile, unsigned short Port = 0);
    string downloadString(const std::string& Host, const std::string& RemoteFile, unsigned short Port = 0);
};

#endif // DOWNLOAD_H
