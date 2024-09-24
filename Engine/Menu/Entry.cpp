#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <fstream>
#include "CoreManager.h"
#include "Entry.h"
#include "../Func.h"

void Entry::init()
{
    StringRepository* strRepo = CoreManager::getInstance().getStrRepo();

    // get fonts
    std::ifstream fontFile("resources/lang/fonts.txt");
    std::string line;

    while(std::getline(fontFile, line))
    {
        std::vector<std::string> param = Func::Split(line, ',');
        std::string name = param[0];
        std::string fontf = param[1];

        strRepo->LoadFontFromFile("resources/font/"+fontf, name);
    }

    // get langs
    std::ifstream langFile("resources/lang/languages.txt");

    while(std::getline(fontFile, line))
    {
        std::vector<std::string> param = Func::Split(line, ',');
        std::string code = param[0];
        std::string name = param[1];
        std::string font = param[2];

        strRepo->LoadLanguageFile(code, name, "resources/lang/"+code+"/"+code+".txt");
        strRepo->langToFontMapping[code] = font;
    }

    SPDLOG_INFO("Entry initialized.");
}

void Entry::draw()
{
    test.setFont("kaku-uni");
    test.setCharacterSize(20);
    test.setString("{color 255 255 255} Thank you for testing.");
    test.setOrigin(test.getLocalBounds().width/2, test.getLocalBounds().height/2);
    test.setPosition(3840/2, 2160/2);
    test.draw();
}