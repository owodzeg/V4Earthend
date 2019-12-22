#include "Download.h"
#include <sstream>
#include <iostream>
#include <dirent.h>

using namespace std;

Download::Download()
{

}

bool Download::dl_file(const std::string& host, const std::string& web_file, const std::string& local_file)
{
    ///make sure all directories create properly
    string sfile;
    string sfull = "";
    vector<string> dirlist;

    stringstream oss(local_file);
    while(getline(oss,sfile,'/'))
    {
        //cout << "directory " << sfile << endl;
        sfull += sfile+"/";
        dirlist.push_back(sfull);
    }

    dirlist.pop_back();
    for(int i=0; i<dirlist.size(); i++)
    {
        _mkdir(dirlist[i].c_str());
    }

    sf::Http http;
    sf::Http::Request request;

    http.setHost(host,0);
    request.setMethod(sf::Http::Request::Get);
    request.setUri(web_file);

    sf::Http::Response page = http.sendRequest(request);//Get the data

    std::string file_str = page.getBody();

    if(file_str.size() <= 0)
    return false;

    std::ofstream file(local_file, std::ios::out | std::ios::binary | std::ios::trunc);
    file.write(file_str.c_str(), file_str.size());//Write the file
    file.close();

    return true;
}

string Download::dl_str_post(const std::string& host, const std::string& file, const std::string& postfields)
{
    sf::Http http;
    sf::Http::Request request;

    http.setHost(host,0);
    request.setMethod(sf::Http::Request::Post);
    request.setUri(file);
    request.setField("Content-Type", "application/x-www-form-urlencoded");
    request.setBody(postfields);

    return http.sendRequest(request).getBody();
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
