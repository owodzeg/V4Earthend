#ifndef WORKER_H
#define WORKER_H

#include <string>
#include <vector>
#include <curl/curl.h>

class Worker {
public:
    std::vector<std::string> servers;
    std::string closestServer = "";

    bool active = true;
    bool busy = false;

    enum Action
    {
        IDLE = 0,
        GET_SERVERS = 1
    };

    Action myAction;

    Worker();
    void init();
    void getAvailableServers();
    void testConnection();
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output, int dl_id);
    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
    std::vector<char> downloadFromUrl(std::string& url);
    void setAction(Action action);
    void listen();
    bool isBusy();
};

#endif //WORKER_H
