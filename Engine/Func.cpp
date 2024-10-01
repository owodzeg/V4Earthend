#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "Func.h"
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <spdlog/spdlog.h>

// Include Native File Dialog for cross-platform directory dialog
#include <fcntl.h>
#include <nfd.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h>
    #include <combaseapi.h>  // for CoInitialize/CoUninitialize
#elif defined(__linux__)
    #include <fstream>
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <unistd.h>
#endif

namespace fs = std::filesystem;

std::vector<std::string> Func::Split(const std::string& s, char delim)
{
    std::vector<std::string> elems;

    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }

    return elems;
}

sf::String Func::ConvertToUtf8String(const std::string& s)
{
    return sf::String::fromUtf8(s.begin(), s.end());
}
std::string Func::trim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string Func::wrap_text(std::string input, int box_width, sf::Font& font, int character_size)
{
    //cout << "wrap_text(" << input << ", " << box_width << ")" << endl;

    if (input.size() <= 0)
        return "";

    std::string temp = "";
    std::vector<std::string> words;

    for (unsigned long i = 0; i < input.size(); i++)
    {
        if ((input[i] == ' ') || (input[i] == '\n') || (i == input.size() - 1))
        {
            if (i == input.size() - 1)
                temp += input[i];

            if (input[i] == '\n')
                temp += input[i];

            //cout << "Registered word: " << temp << endl;
            words.push_back(temp);
            temp = "";
        } else
        {
            temp += input[i];
        }
    }

    std::string full = "";

    std::string prevtemp = "";
    int wordcount = 0;
    temp = "";

    for (unsigned long i = 0; i < words.size(); i++)
    {
        prevtemp = temp;

        if (words[i].find("\n") != std::string::npos)
        {
            //cout << "String found to contain a new line character" << endl;
            std::string prefull = prevtemp + " " + words[i];

            sf::Text t_temp;
            t_temp.setFont(font);
            t_temp.setCharacterSize(character_size);
            t_temp.setString(prefull);

            if (t_temp.getGlobalBounds().width >= box_width)
            {
                full += prevtemp + '\n';
                i--;
            } else
            {
                full += prefull;
            }

            SPDLOG_TRACE("Added \"{}\" to the output", prevtemp + words[i]);

            i++;
            temp = "";
            wordcount = 0;
        }

        if (wordcount > 0)
            temp += " ";

        temp += words[i];
        wordcount++;

        sf::Text t_temp;
        t_temp.setFont(font);
        t_temp.setCharacterSize(character_size);
        t_temp.setString(temp);

        //cout << "Testing string \"" << temp << "\", " << wordcount << " words, size: " << t_temp.getGlobalBounds().width << endl;

        if (t_temp.getGlobalBounds().width >= box_width)
        {
            if (wordcount > 1)
            {
                //cout << "String exceeded the max box width (" << box_width << " vs " << t_temp.getGlobalBounds().width << ")" << endl;
                full += prevtemp;
                full += '\n';

                //cout << "Added \"" << prevtemp << "\" to the output" << endl;

                i -= 1;
                temp = "";
                wordcount = 0;
            } else
            {
                std::string ltemp = "";

                ///if its just a long ass word
                ///need to optimize this because it doesnt work correctly for chinese
                for (unsigned long e = 0; e < temp.size(); e++)
                {
                    ltemp += temp[e];

                    sf::Text t_ltemp;
                    t_ltemp.setFont(font);
                    t_ltemp.setCharacterSize(character_size);
                    t_ltemp.setString(ltemp);

                    if (t_ltemp.getGlobalBounds().width >= box_width - 30)
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

    //cout << "End of word list, adding remains to the full string" << endl;
    full += temp;

    return full;
}

template<typename T>
std::string Func::to_str(const T& t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

std::string Func::num_padding(int num, int padding) ///stolen from cplusplus.com
{
    std::ostringstream ss;
    ss << std::setw(padding) << std::setfill('0') << num;

    return ss.str();
}

void Func::RunExecutable(const std::string& executablePath, const std::vector<std::string>& args) {
#if defined(_WIN32)
    // Windows: Use CreateProcess to run the executable
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    std::string commandLine = "\"" + executablePath + "\"";
    for (const auto& arg : args) {
        commandLine += " \"" + arg + "\"";
    }

    if (CreateProcess(
             executablePath.c_str(), // Application name (path to executable)
             NULL,                // Command line (NULL since we provide the application name)
             NULL,                // Process handle not inheritable
             NULL,                // Thread handle not inheritable
             FALSE,               // Set handle inheritance to FALSE
             0,                   // No creation flags
             NULL,                // Use parent's environment block
             NULL,                // Use parent's starting directory
             &startupInfo,        // Pointer to STARTUPINFO structure
             &processInfo         // Pointer to PROCESS_INFORMATION structure
             )) {
        // Successfully started the process, now close launcher
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    } else {
        SPDLOG_ERROR("Failed to start the process: {}", GetLastError());
    }

#elif defined(__linux__) || defined(__APPLE__)
    pid_t pid = fork();  // Fork a new process
    if (pid == -1) {
        SPDLOG_ERROR("Failed to fork a new process!");
        return;
    }
    if (pid == 0) {  // Child process
        // Prepare arguments for exec
        std::vector<char*> execArgs;
        execArgs.push_back(const_cast<char*>(executablePath.c_str()));
        for (const auto& arg : args) {
            execArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        execArgs.push_back(nullptr);  // Null-terminate the arguments

        execv(execArgs[0], execArgs.data());  // Replace current process with the new one
        SPDLOG_ERROR("Failed to exec the process!");
    } else {
        // Parent process (launcher)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Small delay to ensure the child starts
    }
#endif
}

void Func::CreateDesktopShortcut(const std::string& targetPath, const std::string& description) {
    // Get the Desktop path
    fs::path desktopPath;
#if defined(_WIN32)
    char* userProfile = getenv("USERPROFILE");
    desktopPath = fs::path(userProfile) / "Desktop";
#elif defined(__linux__) || defined(__APPLE__)
    desktopPath = fs::path(getenv("HOME")) / "Desktop";
#endif

#if defined(_WIN32)
    // Windows: Create a .lnk file using IShellLink
    std::wstring shortcutPath = (desktopPath / (description + ".lnk")).wstring();
    std::wstring targetWPath = std::wstring(targetPath.begin(), targetPath.end());

    CoInitialize(NULL);  // Initialize COM
    IShellLinkW* pShellLink = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&pShellLink))) {
        pShellLink->SetPath(targetWPath.c_str());
        pShellLink->SetDescription(std::wstring(description.begin(), description.end()).c_str());

        IPersistFile* pPersistFile = nullptr;
        if (SUCCEEDED(pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile))) {
            pPersistFile->Save(shortcutPath.c_str(), TRUE);
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
    CoUninitialize();  // Clean up COM

#elif defined(__linux__)
    // Linux: Create a .desktop file
    fs::path shortcutPath = desktopPath / (description + ".desktop");
    std::ofstream desktopFile(shortcutPath);
    if (desktopFile.is_open()) {
        desktopFile << "[Desktop Entry]\n";
        desktopFile << "Version=1.0\n";
        desktopFile << "Type=Application\n";
        desktopFile << "Name=" << description << "\n";
        desktopFile << "Exec=" << targetPath << "\n";
        desktopFile << "Icon=utilities-terminal\n";  // You can customize this if you have an icon
        desktopFile << "Terminal=false\n";
        desktopFile.close();

        // Make the shortcut executable
        std::string command = "chmod +x " + shortcutPath.string();
        std::system(command.c_str());
    } else {
        SPDLOG_ERROR("Error creating .desktop file");
    }

#elif defined(__APPLE__)
    // macOS: Use AppleScript to create an alias on the desktop
    std::string appleScript =
        "osascript -e 'tell application \"Finder\" "
        "to make alias file to POSIX file \"" + targetPath + "\" at POSIX file \"" + desktopPath.string() + "\"'";
    std::system(appleScript.c_str());
#endif
}

std::optional<std::string> Func::getHomeDirectory() {
    if (const char* home = std::getenv("HOME")) {
        return std::string(home);
    }
#ifdef _WIN32
    if (const char* homeDrive = std::getenv("HOMEDRIVE")) {
        if (const char* homePath = std::getenv("HOMEPATH")) {
            return std::string(homeDrive) + std::string(homePath);
        }
    }
#endif
    return std::nullopt;
}

std::string Func::getInstallDirectory(const std::string& gameName) {
    std::optional<std::string> homeDirOpt = getHomeDirectory();
    if (!homeDirOpt) {
        throw std::runtime_error("Unable to determine the home directory.");
    }
    std::string homeDir = *homeDirOpt;

#ifdef _WIN32
    std::string installDir = homeDir + "\\AppData\\Local\\" + gameName;
#elif __APPLE__
    std::string installDir = homeDir + "/Library/Application Support/" + gameName;
#elif __linux__
    std::string installDir = homeDir + "/.local/share/" + gameName;
#else
    throw std::runtime_error("Unsupported platform.");
#endif

    fs::create_directories(installDir);
    return installDir;
}

std::string Func::getCurrentWorkingDir()
{
    std::filesystem::path cwd = std::filesystem::current_path();
    return cwd.string();
}

std::optional<std::string> Func::openDirectoryDialog() {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

    if (result == NFD_OKAY) {
        std::string selectedPath(outPath);
        free(outPath);
        return selectedPath;
    } else if (result == NFD_CANCEL) {
        return std::nullopt;
    } else {
        return std::nullopt;
    }
}

void Func::create_directory(const std::string& path) {
    std::filesystem::path dir(path);

    try {
        if (std::filesystem::create_directories(dir)) {
            SPDLOG_INFO("Directory created: {}", dir.string());
        } else {
            SPDLOG_WARN("Directory already exists: {}", dir.string());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        SPDLOG_ERROR("Error creating directory", std::string(e.what()));
    }
}

std::string Func::getTempDirectory() {
#ifdef _WIN32
    // For Windows
    char tempPath[MAX_PATH];
    if (GetTempPathA(MAX_PATH, tempPath)) {
        return std::string(tempPath);
    }
#elif defined(__linux__) || defined(__APPLE__)
    // For Linux and macOS
    const char* tempPath = std::getenv("TMPDIR");
    if (tempPath) {
        return std::string(tempPath);
    }
    return "/tmp"; // Default for Linux
#endif
    return ""; // Fallback if no temp directory found
}

bool Func::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void Func::smoothTransition(float& current, float& destination, float& delta)
{
    if(fabs(current - destination) > 0)
    {
        if(current > destination)
        {
            current -= fabs(current - destination) * delta;
        }
        else
        {
            current += fabs(current - destination) * delta;
        }
    }
}

void Func::openFileExplorer(const std::string& path) {
#ifdef _WIN32
    // Windows: Use the "explorer" command
    std::string command = "explorer \"" + path + "\"";
#elif __linux__
    // Linux: Use "xdg-open" command, which is the standard for opening directories/files
    std::string command = "xdg-open \"" + path + "\"";
#else
    throw std::runtime_error("Unsupported platform");
#endif

    // Execute the command using system()
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("Failed to open file explorer");
    }
}