#include "AnimatedObject.h"
#include "math.h"
#include <fstream>
#include <iostream>
#include "P4A.h"
#include "Object.h"
#include <sstream>

using namespace std;

std::vector<std::string> split2(const std::string &s, char delim)
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

AnimatedObject::AnimatedObject()
{

}

void AnimatedObject::loadAnim(std::string data, P4A handle)
{
    ///LOADANIM ver from 6.10.2019

    objects.clear();

    bool legit = false;
    string version = "";

    istringstream iss(data);
    string line;
    while(getline(iss, line))
    {
        if(line.find("V4Mater-ver-") != std::string::npos)
        {
            version = line.substr(line.find_last_of("-")+1);
            legit = true;

            cout << "Anim format legit. Version " << version << endl;
        }

        if(legit)
        {
            if(version == "1.00")
            {
                if(line.find("S:") != std::string::npos)
                {
                    max_time = atof(line.substr(line.find_first_of(":")+1).c_str());
                    cout << "Time: " << max_time << endl;
                }

                if(line.find("OI:") != std::string::npos)
                {
                    string objectdata = line.substr(line.find_first_of(":")+1);
                    vector<string> object = split2(objectdata,',');

                    string tex_file = object[0];
                    string texture_data = handle.ReadToMemory(tex_file);

                    cout << "texture data: " << texture_data.size() << endl;

                    sf::Texture obj_texture;
                    obj_texture.loadFromMemory(texture_data.data(),texture_data.size());
                    obj_texture.setSmooth(true);

                    int parent = 0; ///unused yet

                    Object tmp;
                    tmp.Load(obj_texture,0,0);
                    tmp.layer = objects.size();
                    tmp.parent = parent;
                    objects.push_back(tmp);

                    cout << "Added new object from " << tex_file << endl;

                }

                if(line.find("F:") != std::string::npos)
                {
                    string framedata = line.substr(line.find_first_of(":")+1);
                    vector<string> frame = split2(framedata,',');

                    string animation_name = frame[0];

                    float time = atof(frame[1].c_str());
                    int objectID = atoi(frame[2].c_str());

                    float pos_x = atof(frame[3].c_str());
                    float pos_y = atof(frame[4].c_str());
                    float rotation = atof(frame[5].c_str());
                    float or_x = atof(frame[6].c_str());
                    float or_y = atof(frame[7].c_str());
                    float scale_x = atof(frame[8].c_str());
                    float scale_y = atof(frame[9].c_str());
                    objects[objectID].SetCustomFrame(time,pos_x,pos_y,or_x,or_y,rotation,scale_x,scale_y);
                }
            }
        }
    }
}

void AnimatedObject::LoadConfig(std::string unitParamPath)
{
    P4A handle;

    if(unitParamPath != "")
    {
        handle.ReadDictionary(unitParamPath);
        string animdata = handle.ReadToMemory("data.anim");

        if(animdata == "")
        {
            cout << "Invalid animation file!" << endl;
        }

        loadAnim(animdata,handle);
    }
}
void AnimatedObject::Draw(sf::RenderWindow& window)
{
    cur_pos += 1 / float(fps);

    if(cur_pos > max_time)
    {
        cur_pos = 0;
    }

    for(int i=0; i<objects.size(); i++)
    {
        objects[i].gx = x;
        objects[i].gy = y;
        objects[i].SetPos(cur_pos);
        objects[i].Draw(window);
    }
}
