#ifndef WORKER_H
#define WORKER_H

#include <string>
#include <vector>
#include <curl/curl.h>

class Worker {
public:
    std::vector<std::string> servers;
    std::string closestServer = "";

    struct File
    {
        std::vector<char> buffer;
        int size = 0;
        int progress = 0;
    };

    std::vector<File> dl_buffer;
    std::vector<std::vector<char>> files;

    bool active = true;
    bool busy = false;

    float currentDLTotal = 0;
    float currentDLProgress = 0;

    float currentTaskTotal = 0;
    float currentTaskProgress = 0;

    enum Action
    {
        IDLE = 0,
        INIT_FIRSTRUN = 1,
        DOWNLOAD_EARTHEND = 2,
        LOAD_UNITS = 3,
        CHECK_HERO_UPDATE = 4,
        DOWNLOAD_HERO = 5,
        RUN_HERO = 6,
        LOGIN = 7,
        REGISTER = 8
    };

    Action myAction;

    std::string platform = "windows";
    std::string gamePath = "";
    std::string execName = "V4Hero";
    std::string launcherExecName = "Patafour";

    struct FileEntry
    {
        std::string name = "";
        int size = 0;
        int progress = 0;
    };

    std::vector<FileEntry> update_files;
    std::vector<FileEntry> downloaded_files;

    std::string cert_file;
    std::string cert_path;

    bool isOffline = false;
    std::string branch = "main";
    std::string hero_version = "";

    int error = -1;
    int rtn = -1;

    bool downloadStarted = false;

    std::string token = "";
    int token_expire = 0;

    bool isSupporter = false;
    std::vector<std::string> all_branches = {"main"};

    Worker();
    void platformSpecific();
    void init();
    void getAvailableServers();
    void testConnection();
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::vector<char>* data);
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
    int testUrl(std::string& url);
    std::vector<char> downloadFromUrl(std::string url, bool addToBuffer = false);
    std::vector<char> downloadFromUrlPost(std::string url, std::vector<std::string> postParams, std::vector<std::string> postValues, bool addToBuffer = false);
    void setAction(Action action);
    void listen();
    bool isBusy();
};

#endif //WORKER_H
