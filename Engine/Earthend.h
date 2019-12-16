#ifndef EARTHEND_H
#define EARTHEND_H

#include "Download.h"
#include "Background.h"
#include "Camera.h"
#include "P4A.h"
#include "AnimatedObject.h"
#include "MD5.h"
#include <iostream>
#include <windows.h>
#include <sstream>

using namespace std;

class Earthend
{
    public:
    Download download;
    Camera camera;
    P4A handle;
    AnimatedObject a_baby;

    sf::Font p4kaku;
    sf::RectangleShape rect_1,rect_2;

    sf::Texture tex_logo,tex_box,tex_sword,tex_pata,tex_eye;
    sf::Sprite s_logo,s_loginbox,s_emailbox,s_passbox;

    sf::Text t_username,t_email,t_password;
    sf::Text t_newsheader,t_news;
    sf::Text t_login,t_create,t_playoffline;
    sf::Text t_version;

    float fps;

    string web_file_list;

    vector<string> web_files; ///web file location
    vector<string> web_hashes; ///web file hash
    vector<string> web_sizes; ///web file size

    vector<string> local_files; ///local file location

    vector<string> files_download; ///file to download
    vector<string> files_remove; ///files to remove

    bool finish = false;
    Background test_bg;

    Earthend();

    void getWebFileList();
    void getLocalFileList(const char* dirn);
    string getFileHash(string filename);
    vector<string> split(const std::string &s, char delim);
    void splitWebFileList();
    void verifyFiles();
    void downloadFile();

    void Init(sf::RenderWindow& window);
};

#endif // EARTHEND_H
