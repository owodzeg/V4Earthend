#include "Earthend.h"
#include <time.h>
#include <cstdlib>
#include <SFML/Graphics.hpp>

using namespace std;

string wrap_text(string input, int box_width, sf::Font font, int character_size)
{
    cout << "wrap_text(" << input << ", " << box_width << ")" << endl;

    string temp = "";
    vector<string> words;

    for(int i=0; i<input.size(); i++)
    {
        if((input[i] == ' ') || (input[i] == '\n') || (i == input.size()-1))
        {
            if(i == input.size()-1)
            temp += input[i];

            if(input[i] == '\n')
            temp += input[i];

            cout << "Registered word: " << temp << endl;
            words.push_back(temp);
            temp = "";
        }
        else
        {
            temp += input[i];
        }
    }

    string full = "";

    string prevtemp = "";
    int wordcount = 0;
    temp = "";

    for(int i=0; i<words.size(); i++)
    {
        prevtemp = temp;

        if(words[i].find("\n") != std::string::npos)
        {
            cout << "String found to contain a new line character" << endl;
            string prefull = prevtemp + " " + words[i];

            sf::Text t_temp;
            t_temp.setFont(font);
            t_temp.setCharacterSize(character_size);
            t_temp.setString(prefull);

            if(t_temp.getGlobalBounds().width >= box_width)
            {
                full += prevtemp + '\n';
                i--;
            }
            else
            {
                full += prefull;
            }

            cout << "Added \"" << prevtemp + words[i] << "\" to the output" << endl;

            i++;
            temp = "";
            wordcount = 0;
        }

        if(wordcount>0)
        temp += " ";

        temp += words[i];
        wordcount++;

        sf::Text t_temp;
        t_temp.setFont(font);
        t_temp.setCharacterSize(character_size);
        t_temp.setString(temp);

        cout << "Testing string \"" << temp << "\", " << wordcount << " words, size: " << t_temp.getGlobalBounds().width << endl;

        if(t_temp.getGlobalBounds().width >= box_width)
        {
            if(wordcount > 1)
            {
                cout << "String exceeded the max box width (" << box_width << " vs " << t_temp.getGlobalBounds().width << ")" << endl;
                full += prevtemp;
                full += '\n';

                cout << "Added \"" << prevtemp << "\" to the output" << endl;

                i -= 1;
                temp = "";
                wordcount = 0;
            }
            else
            {
                string ltemp = "";

                ///if its just a long ass word
                for(int e=0; e<temp.size(); e++)
                {
                    ltemp += temp[e];

                    sf::Text t_ltemp;
                    t_ltemp.setFont(font);
                    t_ltemp.setCharacterSize(character_size);
                    t_ltemp.setString(ltemp);

                    if(t_ltemp.getGlobalBounds().width >= box_width-30)
                    {
                        full += ltemp;
                        full += "-";
                        full += '\n';

                        ltemp = "";
                    }
                }

                ///adding remains
                temp = ltemp;
                wordcount = 1;
            }
        }
    }

    cout << "End of word list, adding remains to the full string" << endl;
    full += temp;

    return full;
}

Earthend::Earthend()
{
    srand(time(NULL));
    int baby = rand() % 8 + 1;

    test_bg.Load("earthend");

    switch(baby)
    {
        case 1:
        {
            a_baby.LoadConfig("resources/units/hatapon.p4a");
            break;
        }
        case 2:
        {
            a_baby.LoadConfig("resources/units/ton.p4a");
            break;
        }
        case 3:
        {
            a_baby.LoadConfig("resources/units/chin.p4a");
            break;
        }
        case 4:
        {
            a_baby.LoadConfig("resources/units/kan.p4a");
            break;
        }
        case 5:
        {
            a_baby.LoadConfig("resources/units/meden.p4a");
            break;
        }
        case 6:
        {
            a_baby.LoadConfig("resources/units/zigo.p4a");
            break;
        }
        case 7:
        {
            a_baby.LoadConfig("resources/units/gong.p4a");
            break;
        }
        case 8:
        {
            a_baby.LoadConfig("resources/units/hero.p4a");
            break;
        }
    }

    rect_1.setSize(sf::Vector2f(1280,720));
    rect_2.setSize(sf::Vector2f(445,720));

    rect_1.setFillColor(sf::Color(0,0,0,100));
    rect_2.setFillColor(sf::Color(0,0,0,128));

    p4kaku.loadFromFile("resources/fonts/p4kaku.ttf");

    tex_logo.loadFromFile("resources/graphics/ui/logo.png");
    tex_box.loadFromFile("resources/graphics/ui/box.png");
    tex_sword.loadFromFile("resources/graphics/ui/sword.png");
    tex_pata.loadFromFile("resources/graphics/ui/pata.png");
    tex_eye.loadFromFile("resources/graphics/ui/eye.png");

    s_logo.setTexture(tex_logo);
    s_logo.setPosition(188,16);

    s_loginbox.setTexture(tex_box);
    s_emailbox.setTexture(tex_box);
    s_passbox.setTexture(tex_box);

    s_loginbox.setOrigin(s_loginbox.getGlobalBounds().width/2,s_loginbox.getGlobalBounds().height/2);
    s_emailbox.setOrigin(s_emailbox.getGlobalBounds().width/2,s_emailbox.getGlobalBounds().height/2);
    s_passbox.setOrigin(s_passbox.getGlobalBounds().width/2,s_passbox.getGlobalBounds().height/2);

    t_username.setFont(p4kaku);
    t_email.setFont(p4kaku);
    t_password.setFont(p4kaku);
    t_newsheader.setFont(p4kaku);
    t_news.setFont(p4kaku);
    t_login.setFont(p4kaku);
    t_create.setFont(p4kaku);
    t_playoffline.setFont(p4kaku);
    t_version.setFont(p4kaku);

    t_username.setFillColor(sf::Color::White);
    t_email.setFillColor(sf::Color::White);
    t_password.setFillColor(sf::Color::White);
    t_newsheader.setFillColor(sf::Color::White);
    t_news.setFillColor(sf::Color::White);
    t_login.setFillColor(sf::Color::White);
    t_create.setFillColor(sf::Color::White);
    t_playoffline.setFillColor(sf::Color::White);
    t_version.setFillColor(sf::Color(255,255,255,128));

    t_username.setString("Username");
    t_email.setString("Email");
    t_password.setString("Password");
    t_newsheader.setString("News feed");
    t_news.setString(wrap_text("8 more days to the first Alpha Demo release.\n\nWe are having some troubles with getting done exactly everything we planned for the first V4 release, so please keep in mind that the game will be constantly updated and the screen you're looking at belongs to the Patafour Launcher, which will make sure to keep you up to date with all new updates and features.\n\nThanks for reading!", 420, p4kaku, 18));
    t_login.setString("Log in");
    t_create.setString("Create new account");
    t_playoffline.setString("Play offline");
    t_version.setString("build V4Hero-1.0.0");

    t_username.setCharacterSize(24);
    t_email.setCharacterSize(24);
    t_password.setCharacterSize(24);
    t_newsheader.setCharacterSize(24);
    t_news.setCharacterSize(18);
    t_login.setCharacterSize(24);
    t_create.setCharacterSize(24);
    t_playoffline.setCharacterSize(24);
    t_version.setCharacterSize(24);

    t_username.setOrigin(t_username.getGlobalBounds().width,t_username.getGlobalBounds().height/2);
    t_email.setOrigin(t_email.getGlobalBounds().width,t_email.getGlobalBounds().height/2);
    t_password.setOrigin(t_password.getGlobalBounds().width,t_password.getGlobalBounds().height/2);
    t_newsheader.setOrigin(t_newsheader.getGlobalBounds().width/2,t_newsheader.getGlobalBounds().height/2);
    t_news.setOrigin(0,0);
    t_login.setOrigin(t_login.getGlobalBounds().width/2,t_login.getGlobalBounds().height/2);
    t_create.setOrigin(t_create.getGlobalBounds().width/2,t_create.getGlobalBounds().height/2);
    t_playoffline.setOrigin(t_playoffline.getGlobalBounds().width/2,t_playoffline.getGlobalBounds().height/2);
    t_version.setOrigin(t_version.getGlobalBounds().width/2,t_version.getGlobalBounds().height/2);
}

void Earthend::getWebFileList()
{
    web_file_list = download.dl_str("dl.patafourgame.com","/sample.txt");
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

string Earthend::getFileHash(string filename)
{
    ifstream file(filename, ifstream::binary);

    ostringstream o;
    o << file.rdbuf();

    file.close();

    string data = o.str();
                    /// duck
                    /// substr(0): duck
                    /// substr(1): uck
                    /// substr(3): k

                    /// substr(0,2): du
                    /// substr(1,3): uck
                    /// substr(2,1): c

                    /// ./resources/earthend.png
                    /// find_last_of("/"): 11
                    /// substr(11 + 1): earthend.png

    string cut = filename.substr(filename.find_last_of("\\")+1);

    return cut+md5(data);
}

vector<string> Earthend::split(const std::string &s, char delim)
{
    ///apple,orange,banana
    ///split()
    ///vector[0] = apple
    ///vector[1] = orange
    ///vector[2] = banana

    std::vector<std::string> elems;

	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	return elems;
}

void Earthend::splitWebFileList()
{
    vector<string> files = split(web_file_list,';');

    for(int i=0; i<files.size(); i++)
    {
        vector<string> file_segments = split(files[i],',');

        cout << "Web location of file: " << file_segments[0] << endl;
        cout << "File hash: " << file_segments[1] << endl;
        cout << "File size: " << file_segments[2] << endl;

        web_files.push_back(file_segments[0]);
        web_hashes.push_back(file_segments[1]);
        web_sizes.push_back(file_segments[2]);
    }
}

void Earthend::verifyFiles()
{
    for(int i=0; i<web_files.size(); i++)
    {
        string filename = web_files[i].substr(web_files[i].find_first_of("/")+1);

        string web_hash = web_hashes[i];
        string web_size = web_sizes[i];

        ifstream local(filename);
        bool exists = local.good();

        ///if file exists - check it's hash if it needs to be updated
        ///if file doesn't exist, add it to download queue

        if(exists == true)
        {
            ///check hash and size
            string local_hash = getFileHash(filename);

            ///if hashes are the same, nothing needs to be updated
            ///if hashes are different, add the file to download queue because it needs to be updated
            if(web_hash != local_hash)
            {
                files_download.push_back(web_files[i]);
            }
        }
        else
        {
            ///add to queue
            files_download.push_back(web_files[i]);
        }
    }

    ///determine all files that needs to be removed
    for(int i=0; i<local_files.size(); i++)
    {
        bool exists = false;

        string local_filename = local_files[i].substr(local_files[i].find_first_of("\\")+1);

        for(int a=0; a<local_filename.size(); a++)
        {
            if(local_filename[a] == '\\')
            {
                local_filename[a] = '/';
            }
        }

        for(int i2=0; i2<web_files.size(); i2++)
        {
            string filename = web_files[i2].substr(web_files[i2].find_first_of("/")+1);

            if(local_filename == filename)
            {
                exists = true;
            }
        }

        if(exists == false)
        {
            files_remove.push_back(local_files[i]);
        }
    }


    cout << "Files to be downloaded:" << endl;

    for(int i=0; i<files_download.size(); i++)
    {
        cout << files_download[i] << endl;
    }

    cout << "Files to be removed: " << endl;

    for(int i=0; i<files_remove.size(); i++)
    {
        cout << files_remove[i] << endl;
    }
}

void Earthend::downloadFile()
{

}

void Earthend::Init(sf::RenderWindow& window)
{
    ///updater code
    camera.Work(window,fps);
    test_bg.setCamera(camera);
    test_bg.Draw(window);

    a_baby.fps = fps;
    a_baby.x += 0.1;
    a_baby.Draw(window);

    /// user interface

    rect_2.setPosition(window.getSize().x-rect_2.getSize().x,0);

    sf::View temp;
    temp = window.getView();

    window.setView(window.getDefaultView());

    window.draw(rect_1);
    window.draw(rect_2);

    window.draw(s_logo);

    s_loginbox.setPosition(480,240);
    s_passbox.setPosition(480,320);

    window.draw(s_loginbox);
    window.draw(s_passbox);

    t_username.setPosition(s_loginbox.getPosition().x-(s_loginbox.getGlobalBounds().width/2)-20,s_loginbox.getPosition().y-4);
    t_password.setPosition(s_passbox.getPosition().x-(s_passbox.getGlobalBounds().width/2)-20,s_passbox.getPosition().y-4);

    window.draw(t_username);
    window.draw(t_password);

    t_login.setPosition(400,388);
    t_create.setPosition(400,422);
    t_playoffline.setPosition(400,458);

    window.draw(t_login);
    window.draw(t_create);
    window.draw(t_playoffline);

    t_newsheader.setPosition(rect_2.getPosition().x+(window.getSize().x-rect_2.getPosition().x)/2,14);
    t_news.setPosition(rect_2.getPosition().x+8,46);

    window.draw(t_newsheader);
    window.draw(t_news);

    window.setView(temp);
}
