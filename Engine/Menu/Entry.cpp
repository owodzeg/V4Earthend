#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include "CoreManager.h"
#include "Entry.h"
#include "../Func.h"

void Entry::init()
{
    StringRepository* strRepo = CoreManager::getInstance().getStrRepo();
    strRepo->LoadFontFromFile("resources/font/P4KakuPop-UNI.ttf", "kaku-uni");

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