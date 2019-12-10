#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <SFML/Network.hpp>
#include <fstream>

using namespace std;

class Download
{
    public:
    Download();

    void dl_file(const std::string& host, const std::string& web_file, const std::string& local_file);
    string dl_str(const std::string& host, const std::string& file);
};

#endif // DOWNLOAD_H
