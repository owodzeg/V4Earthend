#include <iostream>
#include <cstdlib>

#include "Engine/V4Updater.h"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "V4 Updater Initialize" << endl;
    cout << "Working dir: " << argv[0] << endl;

    ///Fix for buggy CreateProcess not copying the working directory properly
    string wd = argv[0];
    wd = wd.substr(0,wd.find_last_of("\\"));
    SetCurrentDirectoryA(wd.c_str());

    V4Updater V4;
    V4.Init();

    return 0;
}
