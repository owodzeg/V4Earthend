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
    std::vector<std::string> files;

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
        DOWNLOAD_EARTHEND = 2
    };

    Action myAction;

    std::string platform = "windows";

    Worker();
    void platformSpecific();
    void init();
    void getAvailableServers();
    void testConnection();
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output, int dl_id);
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
    int testUrl(std::string& url);
    std::vector<char> downloadFromUrl(std::string url, bool addToBuffer = false);
    std::vector<char> downloadFromUrlPost(std::string url, std::vector<std::string> postParams, std::vector<std::string> postValues, bool addToBuffer = false);
    void setAction(Action action);
    void listen();
    bool isBusy();
};

#endif //WORKER_H
