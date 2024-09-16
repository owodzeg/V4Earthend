#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 

#include "Config.h"
#include "Func.h"
#include "V4Earthend.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <spdlog/spdlog.h>
#include "CoreManager.h"

using namespace std;

Config::Config()
{
    SPDLOG_INFO("Config initialization");

    for(auto i=0; i<configDefaults.size(); i++)
        configMap[configKeys[i]] = configDefaults[i];

    SPDLOG_INFO("Config initialization finished");
}

void Config::LoadConfig()
{

}

void Config::SaveConfig()
{

}
void Config::ReloadLanguages()
{

}
int Config::GetInt(std::string key)
{
    int num = atoi(configMap[key].c_str());
    return num;
}

std::string Config::GetString(std::string key)
{
    return configMap[key];
}
void Config::SetString(std::string key, std::string val)
{
    configMap[key] = val;
}

std::string Config::GetLanguageName()
{
    return "";
}

bool Config::keyExists(std::string key)
{
    if (configMap.count(key) == 0)
        return false;
    else
        return true;
}
