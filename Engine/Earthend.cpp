#include "Earthend.h"
#include <time.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <windows.h>
#include <shlobj.h>
#include <dirent.h>

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

static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{

    if(uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char *) lpData;
        std::cout << "path: " << tmp << std::endl;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string BrowseFolder(std::string saved_path)
{
    TCHAR path[MAX_PATH];

    const char * path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle  = ("Browse for folder...");
    bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn       = BrowseCallbackProc;
    bi.lParam     = (LPARAM) path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        //get the name of the folder and put it in path
        SHGetPathFromIDList ( pidl, path );

        //free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }

        return path;
    }

    return "";
}

Earthend::Earthend()
{
    srand(time(NULL));
    int baby = rand() % 8 + 1;

    struct stat info;
    if(stat("resources", &info) != 0)
    {
        cout << "No resources folder to be found, enable firstrun" << endl;
        fr = true;
        state = 0;
    }
    else
    {
        cout << "Resources folder found! Execute the launcher" << endl;
        state = 3;
    }

    TCHAR pf[MAX_PATH];
    SHGetFolderPathA(NULL,CSIDL_PROGRAM_FILES,0,NULL,pf);
    string program_files = string(pf)+"\\Patafour";
    _mkdir(program_files.c_str());
    cout << program_files << endl;
    installdir = program_files;

    ///FIRST-RUN LOAD
    if(fr)
    {
        FR_p4kaku.loadFromMemory(bundle.str_DATA_p4kaku.data(),bundle.str_DATA_p4kaku.size()); ///font

        FR_t_box.loadFromMemory(bundle.str_DATA_dirbox.data(),bundle.str_DATA_dirbox.size());
        FR_t_dots.loadFromMemory(bundle.str_DATA_dirsel.data(),bundle.str_DATA_dirsel.size());
        FR_t_checkbox.loadFromMemory(bundle.str_DATA_checkbox.data(),bundle.str_DATA_checkbox.size());
        FR_t_mark.loadFromMemory(bundle.str_DATA_checkmark.data(),bundle.str_DATA_checkmark.size());
        FR_t_pon.loadFromMemory(bundle.str_DATA_pon.data(),bundle.str_DATA_pon.size());
        FR_t_eye.loadFromMemory(bundle.str_DATA_eye.data(),bundle.str_DATA_eye.size());

        FR_t_pon.setSmooth(true);
        FR_t_eye.setSmooth(true);

        FR_box.setTexture(FR_t_box);
        FR_dots.setTexture(FR_t_dots);
        FR_checkbox.setTexture(FR_t_checkbox);
        FR_mark.setTexture(FR_t_mark);
        FR_checkbox2.setTexture(FR_t_checkbox);
        FR_mark2.setTexture(FR_t_mark);
        FR_pon1.setTexture(FR_t_pon);
        FR_eye1.setTexture(FR_t_eye);
        FR_pon2.setTexture(FR_t_pon);
        FR_eye2.setTexture(FR_t_eye);

        FR_tx_noconnection.setFont(FR_p4kaku);
        FR_tx_retry.setFont(FR_p4kaku);
        FR_tx_firstrun1.setFont(FR_p4kaku);
        FR_tx_firstrun2.setFont(FR_p4kaku);
        FR_tx_installdesc.setFont(FR_p4kaku);
        FR_tx_installdir.setFont(FR_p4kaku);
        FR_tx_desktop.setFont(FR_p4kaku);
        FR_tx_startmenu.setFont(FR_p4kaku);
        FR_tx_install.setFont(FR_p4kaku);
        FR_tx_installinfo.setFont(FR_p4kaku);
        FR_tx_downloading1.setFont(FR_p4kaku);
        FR_tx_downloading2.setFont(FR_p4kaku);
        FR_tx_status.setFont(FR_p4kaku);

        FR_tx_finished.setFont(FR_p4kaku);
        FR_tx_runlauncher.setFont(FR_p4kaku);
        FR_tx_exit.setFont(FR_p4kaku);

        FR_tx_noconnection.setCharacterSize(30);
        FR_tx_retry.setCharacterSize(30);
        FR_tx_firstrun1.setCharacterSize(30);
        FR_tx_firstrun2.setCharacterSize(30);
        FR_tx_installdesc.setCharacterSize(20);
        FR_tx_installdir.setCharacterSize(34);
        FR_tx_desktop.setCharacterSize(30);
        FR_tx_startmenu.setCharacterSize(30);
        FR_tx_install.setCharacterSize(40);
        FR_tx_installinfo.setCharacterSize(14);
        FR_tx_downloading1.setCharacterSize(30);
        FR_tx_downloading2.setCharacterSize(30);
        FR_tx_status.setCharacterSize(30);
        FR_tx_finished.setCharacterSize(30);
        FR_tx_runlauncher.setCharacterSize(24);
        FR_tx_exit.setCharacterSize(30);

        FR_tx_noconnection.setString("Failed to connect to the Patafour Server.");
        FR_tx_retry.setString("Retry");
        FR_tx_firstrun1.setString("It's your first time running Patafour!");
        FR_tx_firstrun2.setString("We need to configure a couple of things first.");
        FR_tx_installdesc.setString("Patafour's installation directory. Don't change it if you're unsure.");
        FR_tx_desktop.setString("Create a Desktop shortcut?");
        FR_tx_startmenu.setString("Add an entry to Start Menu?");
        FR_tx_install.setString("Install");
        FR_tx_installinfo.setString("*Internet connection is required to install the launcher. (Approx. 20 MB)");
        FR_tx_downloading1.setString("Patafour Launcher is downloading required files.");
        FR_tx_downloading2.setString("Please wait...");
        FR_tx_status.setString("Retrieving file list");
        FR_tx_finished.setString("Patafour Launcher installed successfully!");
        FR_tx_runlauncher.setString("Run Patafour Launcher now");
        FR_tx_exit.setString("Exit");

        FR_tx_noconnection.setFillColor(sf::Color::White);
        FR_tx_retry.setFillColor(sf::Color::White);
        FR_tx_firstrun1.setFillColor(sf::Color::White);
        FR_tx_firstrun2.setFillColor(sf::Color::White);
        FR_tx_installdesc.setFillColor(sf::Color::White);
        FR_tx_installdir.setFillColor(sf::Color::White);
        FR_tx_desktop.setFillColor(sf::Color::White);
        FR_tx_startmenu.setFillColor(sf::Color::White);
        FR_tx_install.setFillColor(sf::Color::White);
        FR_tx_installinfo.setFillColor(sf::Color::White);
        FR_tx_downloading1.setFillColor(sf::Color::White);
        FR_tx_downloading2.setFillColor(sf::Color::White);
        FR_tx_status.setFillColor(sf::Color::White);
        FR_tx_finished.setFillColor(sf::Color::White);
        FR_tx_runlauncher.setFillColor(sf::Color::White);
        FR_tx_exit.setFillColor(sf::Color::White);

        FR_pon1.setOrigin(FR_pon1.getGlobalBounds().width/2,FR_pon1.getGlobalBounds().height/2);
        FR_eye1.setOrigin(FR_eye1.getGlobalBounds().width-4,FR_eye1.getGlobalBounds().height-4);

        FR_pon2.setOrigin(FR_pon2.getGlobalBounds().width/2,FR_pon2.getGlobalBounds().height/2);
        FR_eye2.setOrigin(FR_eye2.getGlobalBounds().width-4,FR_eye2.getGlobalBounds().height-4);
        ///FIRST-RUN END
    }
    else
    {
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

        tex_pata.loadFromFile("resources/graphics/ui/pon.png");
        tex_eye.loadFromFile("resources/graphics/ui/eye.png");

        tex_pata.setSmooth(true);
        tex_eye.setSmooth(true);

        pon1.setTexture(tex_pata);
        eye1.setTexture(tex_eye);
        pon2.setTexture(tex_pata);
        eye2.setTexture(tex_eye);

        pon1.setOrigin(pon1.getGlobalBounds().width/2,pon1.getGlobalBounds().height/2);
        eye1.setOrigin(eye1.getGlobalBounds().width-4,eye1.getGlobalBounds().height-4);

        pon2.setOrigin(pon2.getGlobalBounds().width/2,pon2.getGlobalBounds().height/2);
        eye2.setOrigin(eye2.getGlobalBounds().width-4,eye2.getGlobalBounds().height-4);

        rect_1.setSize(sf::Vector2f(1280,720));
        rect_2.setSize(sf::Vector2f(445,720));

        rect_1.setFillColor(sf::Color(0,0,0,100));
        rect_2.setFillColor(sf::Color(0,0,0,128));

        p4kaku.loadFromFile("resources/fonts/p4kaku.ttf");
        p4kaku.loadFromMemory(bundle.str_DATA_p4kaku.data(),bundle.str_DATA_p4kaku.size());

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
        t_news.setString(wrap_text("Only 2 days left to the first Alpha Demo release.\n\nWe are having some troubles with getting done exactly everything we planned for the first V4 release, so please keep in mind that the game will be constantly updated and the screen you're looking at belongs to the Patafour Launcher, which will make sure to keep you up to date with all new updates and features.\n\nThanks for reading!", 420, p4kaku, 18));
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

        ///more launcher crap
        t_checkingupdates.setFont(p4kaku);
        t_updatefound1.setFont(p4kaku);
        t_updatefound2.setFont(p4kaku);
        t_updating.setFont(p4kaku);
        t_updating_per.setFont(p4kaku);
        t_updating_file.setFont(p4kaku);
        t_onlinewarning.setFont(p4kaku);
        t_noconnection1.setFont(p4kaku);
        t_noconnection2.setFont(p4kaku);
        t_tryagain.setFont(p4kaku);
        t_yes.setFont(p4kaku);
        t_no.setFont(p4kaku);

        t_checkingupdates.setFillColor(sf::Color::White);
        t_updatefound1.setFillColor(sf::Color::White);
        t_updatefound2.setFillColor(sf::Color::White);
        t_updating.setFillColor(sf::Color::White);
        t_updating_per.setFillColor(sf::Color::White);
        t_updating_file.setFillColor(sf::Color::White);
        t_onlinewarning.setFillColor(sf::Color::White);
        t_noconnection1.setFillColor(sf::Color::White);
        t_noconnection2.setFillColor(sf::Color::White);
        t_tryagain.setFillColor(sf::Color::White);
        t_yes.setFillColor(sf::Color::White);
        t_no.setFillColor(sf::Color::White);

        t_checkingupdates.setCharacterSize(30);
        t_updatefound1.setCharacterSize(30);
        t_updatefound2.setCharacterSize(30);
        t_updating.setCharacterSize(30);
        t_updating_per.setCharacterSize(30);
        t_updating_file.setCharacterSize(30);
        t_onlinewarning.setCharacterSize(30);
        t_noconnection1.setCharacterSize(30);
        t_noconnection2.setCharacterSize(30);
        t_tryagain.setCharacterSize(30);
        t_yes.setCharacterSize(30);
        t_no.setCharacterSize(30);

        t_checkingupdates.setString("Checking for updates...");
        t_updatefound1.setString("New update has been found!");
        t_updatefound2.setString("Update the game now?");
        t_updating.setString("Updating the game...");
        t_updating_per.setString("");
        t_updating_file.setString("");
        t_onlinewarning.setString("Online features will be disabled when running outdated\nversion of the game. Proceed anyway?");
        t_noconnection1.setString("Could not connect to the Patafour update server.");
        t_noconnection2.setString("Check your internet connection and try again.");
        t_tryagain.setString("Try again");
        t_yes.setString("Yes");
        t_no.setString("No");
    }
}

bool Earthend::file_exists(string file)
{
    struct stat buffer;
    return (stat(file.c_str(), &buffer) == 0);
}

/**void Earthend::getLocalFileList(const char* dirn)
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
}*/

string Earthend::getFileHash(string filename)
{
    ifstream file(filename, ifstream::binary);

    ostringstream o;
    o << file.rdbuf();

    file.close();

    string data = o.str();

    string cut = filename.substr(filename.find_last_of("\\/")+1);

    return cut+md5(data);
}

vector<string> Earthend::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;

	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	return elems;
}

void Earthend::FirstRunDownload()
{
    timeout_clock.restart();

    FR_tx_status.setString("Checking latest version...");
    string version = download.dl_str_post("dl.patafourgame.com","/getversion.php","product_id=earthend");

    if(force_exit)
    {
        state = -1;
    }

    cout << version << endl;
    if((version == "") && (state != -1))
    {
        state = -1;
    }
    else
    {
        FR_tx_status.setString("Retrieving file list...");
        string f_list = download.dl_str_post("dl.patafourgame.com","/getfiles.php","product_id=earthend&product_ver="+version);
        //cout << "Files: " << endl << f_list << endl;

        if(force_exit)
        {
            state = -1;
        }

        if((f_list == "") && (state != -1))
        {
            state = -1;
        }
        else
        {
            string file,start_dir;
            vector<string> web_file,web_hash,loc_file;

            stringstream oss(f_list);
            while(getline(oss,file,'\n'))
            {
                if(file.find("START_DIR:") != std::string::npos)
                {
                    start_dir = file.substr(file.find_first_of(":")+1);
                }
                else if(file.find("TOTAL_SIZE:") != std::string::npos)
                {
                    string str_total = file.substr(file.find_first_of(":")+1);
                    total_size = atoi(str_total.c_str());
                }
                else
                {
                    cout << file << endl;
                    vector<string> f_params = split(file,',');

                    string fname = f_params[0];

                    web_file.push_back(fname);
                    fname = fname.substr(fname.find(start_dir) + start_dir.size());
                    loc_file.push_back(fname);
                    web_hash.push_back(f_params[1]);
                }
            }

            for(int i=0; i<loc_file.size(); i++)
            {
                if(force_exit)
                {
                    state = -1;
                    break;
                }

                bool shouldUpdate = true;

                if(file_exists(installdir+"\\"+loc_file[i]))
                {
                    string cur_hash = getFileHash(installdir+"\\"+loc_file[i]);
                    if(cur_hash == web_hash[i])
                    {
                        cout << "File already exists and doesn't need a replacement." << endl;
                        shouldUpdate = false;
                    }
                    else
                    {
                        cout << "File has a different signature therefore it has been changed." << endl;
                    }
                }

                if(shouldUpdate)
                {
                    FR_tx_status.setString("Downloading "+loc_file[i]);
                    if(!download.dl_file("dl.patafourgame.com",web_file[i],installdir+"\\"+loc_file[i]))
                    {
                        state = -1;
                        break;
                    }

                    string cur_hash = getFileHash(installdir+"\\"+loc_file[i]);
                    cout << cur_hash << " " << web_hash[i];

                    ///check the hashes
                    if(cur_hash == web_hash[i])
                    {
                        cout << " verified!" << endl;
                    }
                    else
                    {
                        cout << " incorrect. Error! Redownload file." << endl;
                        i--;
                    }
                }
            }

            if(state != -1)
            {
                if(force_exit)
                {
                    state = -1;
                }

                if(state != -1)
                {
                    FR_tx_status.setString("Finalizing...");

                    ///create shortcut
                    ofstream nircmd("nircmdc.exe", ios::binary | ios::trunc);
                    nircmd.write(bundle.str_DATA_nircmdc.data(), bundle.str_DATA_nircmdc.size());
                    nircmd.close();

                    string scmd = "nircmdc.exe shortcut \""+installdir+"\\Patafour.exe"+"\" \"~$folder.desktop$\" \"Patafour\"";
                    string scmd2 = "nircmdc.exe shortcut \""+installdir+"\\Patafour.exe"+"\" \"~$folder.programs$\\Patafour\" \"Launch Patafour\"";
                    if(b_desktop)
                    system(scmd.c_str());
                    if(b_startmenu)
                    system(scmd2.c_str());

                    system("del nircmdc.exe");

                    restart_clock.restart();
                    while(restart_clock.getElapsedTime().asSeconds() < 3)
                    {
                        FR_tx_status.setString("Download finished!");
                    }

                    state = 2;
                }
            }
        }
    }
}

void Earthend::CheckForUpdates()
{
    ///Check updater first
    string updater_version = download.dl_str_post("dl.patafourgame.com","/getversion.php","product_id=earthend");

    if(updater_version != launcher_ver)
    {
        if(updater_version != "")
        {
            cout << "Launcher needs updating. Force update" << endl;
            state = 4;
        }
        else
        {
            cout << "No internet connection!" << endl;
            state = 9;
        }
    }
    else
    {
        cout << "Launcher is up to date!" << endl;

        ///Check if game exists
        struct stat info;
        if(stat("game", &info) != 0)
        {
            cout << "No game folder to be found, install the game fresh" << endl;
            state = 7;
        }
        else
        {
            cout << "Game folder found! Compare versions (check md5 of V4Hero.exe)" << endl;

            string loc_hash = getFileHash("game\\V4Hero.exe");
            string hero_version = download.dl_str_post("dl.patafourgame.com","/getversion.php","product_id=hero");
            string web_hash = download.dl_str_post("dl.patafourgame.com","/gethash.php","product_id=hero&product_ver="+hero_version+"&product_file=V4Hero.exe");

            if(loc_hash == web_hash)
            {
                cout << "Game is up to date, continue" << endl;
                state = 10;
            }
            else
            {
                if(web_hash != "")
                {
                    cout << "Game is outdated. Ask for updating." << endl;
                    state = 5;
                }
                else
                {
                    cout << "No internet connection!" << endl;
                    state = 9;
                }
            }
        }
    }
}

void Earthend::UpdateProduct(string productID)
{

}

void Earthend::Init(sf::RenderWindow& window)
{
    switch(state)
    {
        case -1:
        {
            window.clear(sf::Color(64,64,64));

            FR_tx_noconnection.setOrigin(FR_tx_noconnection.getGlobalBounds().width/2,FR_tx_noconnection.getGlobalBounds().height/2);
            FR_tx_retry.setOrigin(FR_tx_retry.getGlobalBounds().width/2,FR_tx_retry.getGlobalBounds().height/2);

            FR_tx_noconnection.setPosition(window.getSize().x/2,window.getSize().y/2-40);
            FR_tx_retry.setPosition(window.getSize().x/2,window.getSize().y/2+10);

            FR_tx_retry.setFillColor(sf::Color::White);

            if(mouseX > FR_tx_retry.getPosition().x-FR_tx_retry.getGlobalBounds().width/2+5)
            {
                if(mouseX < FR_tx_retry.getPosition().x+FR_tx_retry.getGlobalBounds().width/2+5)
                {
                    if(mouseY > FR_tx_retry.getPosition().y-FR_tx_retry.getGlobalBounds().height/2+5)
                    {
                        if(mouseY < FR_tx_retry.getPosition().y+FR_tx_retry.getGlobalBounds().height/2+10)
                        {
                            FR_tx_retry.setFillColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                state = 0;
                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            window.draw(FR_tx_noconnection);
            window.draw(FR_tx_retry);

            break;
        }

        case 0:
        {
            window.clear(sf::Color(64,64,64));

            string viewable_dir = installdir;
            for(int i=0; i<viewable_dir.size(); i++)
            {
                if(viewable_dir[i] == '\\')
                viewable_dir[i] = '/';
            }

            FR_tx_installdir.setString(viewable_dir);

            FR_tx_firstrun1.setOrigin(FR_tx_firstrun1.getGlobalBounds().width/2,FR_tx_firstrun1.getGlobalBounds().height/2);
            FR_tx_firstrun2.setOrigin(FR_tx_firstrun2.getGlobalBounds().width/2,FR_tx_firstrun2.getGlobalBounds().height/2);
            FR_tx_installdesc.setOrigin(0,FR_tx_installdesc.getGlobalBounds().height/2);
            FR_tx_installdir.setOrigin(0,FR_tx_installdir.getGlobalBounds().height/2);
            FR_tx_desktop.setOrigin(0,FR_tx_desktop.getGlobalBounds().height/2);
            FR_tx_startmenu.setOrigin(0,FR_tx_startmenu.getGlobalBounds().height/2);
            FR_tx_install.setOrigin(FR_tx_install.getGlobalBounds().width/2,FR_tx_install.getGlobalBounds().height/2);
            FR_tx_installinfo.setOrigin(FR_tx_installinfo.getGlobalBounds().width/2,FR_tx_installinfo.getGlobalBounds().height/2);

            FR_tx_firstrun1.setPosition(window.getSize().x/2,70);
            FR_tx_firstrun2.setPosition(window.getSize().x/2,104);

            FR_box.setOrigin(FR_box.getGlobalBounds().width/2,FR_box.getGlobalBounds().height/2);
            FR_box.setPosition(window.getSize().x/2,280);

            FR_tx_installdesc.setPosition(FR_box.getPosition().x-FR_box.getGlobalBounds().width/2+16,214);
            FR_tx_installdir.setPosition(FR_box.getPosition().x-FR_box.getGlobalBounds().width/2+20,274);

            FR_dots.setOrigin(FR_dots.getGlobalBounds().width/2,FR_dots.getGlobalBounds().height/2);
            FR_dots.setPosition(FR_box.getPosition().x+FR_box.getGlobalBounds().width/2-62,280);

            FR_checkbox.setPosition(FR_box.getPosition().x-FR_box.getGlobalBounds().width/2,350);
            FR_mark.setPosition(FR_box.getPosition().x-FR_box.getGlobalBounds().width/2+8,358);
            FR_tx_desktop.setPosition(FR_checkbox.getPosition().x+58,368);

            FR_checkbox2.setPosition(FR_box.getPosition().x-FR_box.getGlobalBounds().width/2,410);
            FR_mark2.setPosition(FR_box.getPosition().x-FR_box.getGlobalBounds().width/2+8,418);
            FR_tx_startmenu.setPosition(FR_checkbox.getPosition().x+58,428);

            FR_tx_install.setPosition(window.getSize().x/2,600);
            FR_tx_installinfo.setPosition(window.getSize().x/2,636);

            FR_dots.setColor(sf::Color::White);

            if(mouseX > FR_dots.getPosition().x-FR_dots.getGlobalBounds().width/2)
            {
                if(mouseX < FR_dots.getPosition().x+FR_dots.getGlobalBounds().width/2)
                {
                    if(mouseY > FR_dots.getPosition().y-FR_dots.getGlobalBounds().height/2)
                    {
                        if(mouseY < FR_dots.getPosition().y+FR_dots.getGlobalBounds().height/2)
                        {
                            FR_dots.setColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                string new_installdir = BrowseFolder(installdir);

                                if(new_installdir.size() > 0)
                                installdir = new_installdir;

                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            FR_tx_desktop.setFillColor(sf::Color::White);

            if(mouseX > FR_checkbox.getPosition().x)
            {
                if(mouseX < FR_checkbox.getPosition().x+FR_checkbox.getGlobalBounds().width+FR_tx_desktop.getGlobalBounds().width+10)
                {
                    if(mouseY > FR_checkbox.getPosition().y)
                    {
                        if(mouseY < FR_checkbox.getPosition().y+FR_checkbox.getGlobalBounds().height)
                        {
                            FR_tx_desktop.setFillColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                b_desktop = !b_desktop;
                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            FR_tx_startmenu.setFillColor(sf::Color::White);

            if(mouseX > FR_checkbox2.getPosition().x)
            {
                if(mouseX < FR_checkbox2.getPosition().x+FR_checkbox2.getGlobalBounds().width+FR_tx_startmenu.getGlobalBounds().width+10)
                {
                    if(mouseY > FR_checkbox2.getPosition().y)
                    {
                        if(mouseY < FR_checkbox2.getPosition().y+FR_checkbox2.getGlobalBounds().height)
                        {
                            FR_tx_startmenu.setFillColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                b_startmenu = !b_startmenu;
                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            FR_tx_install.setFillColor(sf::Color::White);

            if(mouseX > FR_tx_install.getPosition().x-FR_tx_install.getGlobalBounds().width/2+5)
            {
                if(mouseX < FR_tx_install.getPosition().x+FR_tx_install.getGlobalBounds().width/2+5)
                {
                    if(mouseY > FR_tx_install.getPosition().y-FR_tx_install.getGlobalBounds().height/2+5)
                    {
                        if(mouseY < FR_tx_install.getPosition().y+FR_tx_install.getGlobalBounds().height/2+10)
                        {
                            FR_tx_install.setFillColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                state = 1;
                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            window.draw(FR_box);
            window.draw(FR_dots);
            window.draw(FR_checkbox);
            if(b_desktop)
            window.draw(FR_mark);

            window.draw(FR_checkbox2);
            if(b_startmenu)
            window.draw(FR_mark2);

            window.draw(FR_tx_firstrun1);
            window.draw(FR_tx_firstrun2);
            window.draw(FR_tx_installdesc);
            window.draw(FR_tx_installdir);
            window.draw(FR_tx_desktop);
            window.draw(FR_tx_startmenu);
            window.draw(FR_tx_install);
            window.draw(FR_tx_installinfo);

            break;
        }

        case 1:
        {
            if(!tlaunched)
            {
                downloadThread = std::thread(Earthend::FirstRunDownload,this);
                tlaunched = true;
            }

            window.clear(sf::Color(64,64,64));

            FR_tx_downloading1.setOrigin(FR_tx_downloading1.getGlobalBounds().width/2,FR_tx_downloading1.getGlobalBounds().height/2);
            FR_tx_downloading2.setOrigin(FR_tx_downloading2.getGlobalBounds().width/2,FR_tx_downloading2.getGlobalBounds().height/2);
            FR_tx_status.setOrigin(FR_tx_status.getGlobalBounds().width/2,FR_tx_status.getGlobalBounds().height/2);

            FR_tx_downloading1.setPosition(window.getSize().x/2,window.getSize().y/2-40);
            FR_tx_downloading2.setPosition(window.getSize().x/2,window.getSize().y/2-10);
            FR_tx_status.setPosition(window.getSize().x/2,window.getSize().y/2+60);

            FR_pon1.setPosition(window.getSize().x/2 - FR_tx_status.getGlobalBounds().width/2 - 40,window.getSize().y/2+60);
            FR_eye1.setPosition(window.getSize().x/2 - FR_tx_status.getGlobalBounds().width/2 - 40,window.getSize().y/2+74);

            FR_pon2.setPosition(window.getSize().x/2 + FR_tx_status.getGlobalBounds().width/2 + 40,window.getSize().y/2+60);
            FR_eye2.setPosition(window.getSize().x/2 + FR_tx_status.getGlobalBounds().width/2 + 40,window.getSize().y/2+74);

            FR_eye1.rotate(1);
            FR_eye2.setRotation(FR_eye1.getRotation() * (-1) + 90);

            window.draw(FR_tx_downloading1);
            window.draw(FR_tx_downloading2);
            window.draw(FR_tx_status);
            window.draw(FR_pon1);
            window.draw(FR_eye1);
            window.draw(FR_pon2);
            window.draw(FR_eye2);

            break;
        }

        case 2:
        {
            tlaunched = false;

            window.clear(sf::Color(64,64,64));

            FR_tx_finished.setOrigin(FR_tx_finished.getGlobalBounds().width/2,FR_tx_finished.getGlobalBounds().height/2);
            FR_tx_runlauncher.setOrigin(0,FR_tx_runlauncher.getGlobalBounds().height/2);
            FR_tx_exit.setOrigin(FR_tx_exit.getGlobalBounds().width/2,FR_tx_exit.getGlobalBounds().height/2);

            FR_tx_finished.setPosition(window.getSize().x/2,window.getSize().y/2-80);

            FR_checkbox.setPosition(450,window.getSize().y/2-40);
            FR_mark.setPosition(FR_checkbox.getPosition().x+8,window.getSize().y/2-32);

            FR_tx_runlauncher.setPosition(FR_checkbox.getPosition().x+58,window.getSize().y/2-22);

            FR_tx_exit.setPosition(window.getSize().x/2,window.getSize().y/2+100);

            FR_tx_runlauncher.setFillColor(sf::Color::White);

            if(mouseX > FR_checkbox.getPosition().x)
            {
                if(mouseX < FR_checkbox.getPosition().x+FR_checkbox.getGlobalBounds().width+FR_tx_runlauncher.getGlobalBounds().width+10)
                {
                    if(mouseY > FR_checkbox.getPosition().y)
                    {
                        if(mouseY < FR_checkbox.getPosition().y+FR_checkbox.getGlobalBounds().height)
                        {
                            FR_tx_runlauncher.setFillColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                b_run = !b_run;
                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            FR_tx_exit.setFillColor(sf::Color::White);

            if(mouseX > FR_tx_exit.getPosition().x-FR_tx_exit.getGlobalBounds().width/2+5)
            {
                if(mouseX < FR_tx_exit.getPosition().x+FR_tx_exit.getGlobalBounds().width/2+5)
                {
                    if(mouseY > FR_tx_exit.getPosition().y-FR_tx_exit.getGlobalBounds().height/2+5)
                    {
                        if(mouseY < FR_tx_exit.getPosition().y+FR_tx_exit.getGlobalBounds().height/2+10)
                        {
                            FR_tx_exit.setFillColor(sf::Color::Green);

                            if(mouseLeftClick)
                            {
                                exit_launcher = true;
                                mouseLeftClick = false;
                            }
                        }
                    }
                }
            }

            window.draw(FR_checkbox);
            if(b_run)
            window.draw(FR_mark);

            window.draw(FR_tx_finished);
            window.draw(FR_tx_runlauncher);
            window.draw(FR_tx_exit);

            break;
        }

        case 3: ///Checking for updates
        {
            if(!tlaunched)
            {
                downloadThread = std::thread(Earthend::CheckForUpdates,this);
                tlaunched = true;
            }

            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            sf::View temp;
            temp = window.getView();

            window.setView(window.getDefaultView());

            window.draw(rect_1);

            t_checkingupdates.setString("Checking for updates...");

            t_checkingupdates.setOrigin(t_checkingupdates.getGlobalBounds().width/2,t_checkingupdates.getGlobalBounds().height/2);
            t_checkingupdates.setPosition(window.getSize().x/2,window.getSize().y/2-20);

            pon1.setPosition(window.getSize().x/2 - t_checkingupdates.getGlobalBounds().width/2 - 40,window.getSize().y/2-26);
            eye1.setPosition(window.getSize().x/2 - t_checkingupdates.getGlobalBounds().width/2 - 40,window.getSize().y/2-12);

            pon2.setPosition(window.getSize().x/2 + t_checkingupdates.getGlobalBounds().width/2 + 40,window.getSize().y/2-26);
            eye2.setPosition(window.getSize().x/2 + t_checkingupdates.getGlobalBounds().width/2 + 40,window.getSize().y/2-12);

            eye1.rotate(1);
            eye2.setRotation(eye1.getRotation() * (-1) + 90);

            window.draw(t_checkingupdates);
            window.draw(pon1);
            window.draw(eye1);
            window.draw(pon2);
            window.draw(eye2);

            window.setView(temp);

            break;

        }

        case 4: ///Updating the launcher
        {
            if(!t2launched)
            {
                downloadThread = std::thread(Earthend::UpdateProduct,this);
                t2launched = true;
            }

            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            sf::View temp;
            temp = window.getView();

            window.setView(window.getDefaultView());

            window.draw(rect_1);

            t_updating.setString("Checking for updates...");

            t_updating.setOrigin(t_updating.getGlobalBounds().width/2,t_updating.getGlobalBounds().height/2);
            t_updating.setPosition(window.getSize().x/2,window.getSize().y/2-20);

            pon1.setPosition(window.getSize().x/2 - t_updating.getGlobalBounds().width/2 - 40,window.getSize().y/2-26);
            eye1.setPosition(window.getSize().x/2 - t_updating.getGlobalBounds().width/2 - 40,window.getSize().y/2-12);

            pon2.setPosition(window.getSize().x/2 + t_updating.getGlobalBounds().width/2 + 40,window.getSize().y/2-26);
            eye2.setPosition(window.getSize().x/2 + t_updating.getGlobalBounds().width/2 + 40,window.getSize().y/2-12);

            eye1.rotate(1);
            eye2.setRotation(eye1.getRotation() * (-1) + 90);

            window.draw(t_checkingupdates);
            window.draw(pon1);
            window.draw(eye1);
            window.draw(pon2);
            window.draw(eye2);

            window.setView(temp);

            break;
        }

        case 5: ///Update found
        {
            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            break;
        }

        case 6: ///Online warning
        {
            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            break;

        }

        case 7: ///Updating game
        {
            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            break;

        }

        case 8: ///Game updated
        {
            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            break;

        }

        case 9: ///Connection error
        {
            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
            a_baby.Draw(window);

            break;
        }

        case 10: ///Login screen
        {
            ///updater code
            camera.Work(window,fps);
            test_bg.setCamera(camera);
            test_bg.Draw(window);

            a_baby.fps = fps;
            a_baby.x += 25 / float(fps);
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

            break;
        }
    }
}

Earthend::~Earthend()
{
    force_exit = true;

    if(downloadThread.joinable())
    downloadThread.join();
}
