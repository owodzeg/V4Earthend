#include "Download.h"


Download::Download()
{

}

void Download::downloadFile(const std::string& Host, const std::string& RemoteFile, const std::string& LocalFile, unsigned short Port)
{
    sf::Http Http;
    sf::Http::Request Request;
    unsigned int FileSize = 0;

    Http.setHost(Host,Port);
    Request.setMethod(sf::Http::Request::Get);
    Request.setUri(RemoteFile);

    sf::Http::Response Page = Http.sendRequest(Request);//Get the data

    FileSize = Page.getBody().size();//Get the size that was returned

    std::string FileContainer = Page.getBody();
    std::ofstream File(LocalFile.c_str(), std::ios::out | std::ios::binary);
    File.write(FileContainer.c_str(), FileSize);//Write the file
    File.close();
}

string Download::downloadString(const std::string& Host, const std::string& RemoteFile, unsigned short Port)
{
    sf::Http Http;
    sf::Http::Request Request;
    unsigned int FileSize = 0;

    Http.setHost(Host,Port);
    Request.setMethod(sf::Http::Request::Get);
    Request.setUri(RemoteFile);

    sf::Http::Response Page = Http.sendRequest(Request);//Get the data

    FileSize = Page.getBody().size();//Get the size that was returned

    std::string FileContainer = Page.getBody();

    return FileContainer;

    ///Unfinished, doesn't work, doesn't use!
}
