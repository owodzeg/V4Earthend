#include "Earthend.h"
#include <SFML/Graphics.hpp>

using namespace std;

Earthend::Earthend()
{
    test_bg.Load("earthend");
    a_baby.LoadConfig("resources/units/meden.p4a");
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
}
