#include "Download.h"


Download::Download()
{

}

void Download::dl_file(const std::string& host, const std::string& web_file, const std::string& local_file)
{
    sf::Http http;
    sf::Http::Request request;

    http.setHost(host,0);
    request.setMethod(sf::Http::Request::Get);
    request.setUri(web_file);

    sf::Http::Response page = http.sendRequest(request);//Get the data

    std::string file_str = page.getBody();
    std::ofstream file(local_file, std::ios::out | std::ios::binary);
    file.write(file_str.c_str(), file_str.size());//Write the file
    file.close();
}

///Retrieve a string value from web
///dl_str("dl.patafourgame.com","/sample.txt");
string Download::dl_str(const std::string& host, const std::string& file)
{
    sf::Http http;
    sf::Http::Request request;

    http.setHost(host,0);
    request.setMethod(sf::Http::Request::Get);
    request.setUri(file);

    return http.sendRequest(request).getBody();
}
