#include "Worker.h"
#include <spdlog/spdlog.h>
#include <fstream>

Worker::Worker()
{

}

void Worker::init()
{
    SPDLOG_INFO("Worker is alive!");

    while(active)
        listen();
}

void Worker::getAvailableServers()
{
    std::string listUrl = "https://dl.patafourgame.com/earthend/essentials/serverList.txt";

    std::ifstream backup("backupServerList.txt");
    if(backup.good())
    {
        SPDLOG_INFO("Reading servers from backup");
    }

    auto data = downloadFromUrl(listUrl);
}

void Worker::testConnection()
{

}

size_t Worker::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output, int dl_id) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

int Worker::ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (dltotal > 0) {
        double progress = (double)dlnow / (double)dltotal * 100;
        SPDLOG_INFO("Download Progress: {:.2f}%", progress);
    } else {
        SPDLOG_INFO("Download Progress: 0%");
    }
    return 0; // Returning 0 tells libcurl to continue downloading
}

std::vector<char> Worker::downloadFromUrl(std::string& url)
{
    SPDLOG_INFO("Attempting to download {}", url);

    std::string responseString;

    // Initialize CURL
    CURL* curl = curl_easy_init();

    if (curl) {
        CURLcode res;

        // Set the URL for the HTTPS request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Use a callback function to capture the response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Worker::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        // Set progress function
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, Worker::ProgressCallback);

        // Enable progress meter (since CURLOPT_NOPROGRESS is disabled by default in newer versions)
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            SPDLOG_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
        else
            SPDLOG_INFO("Response: {}", responseString);

        // Clean up CURL
        curl_easy_cleanup(curl);
    }

    return std::vector<char>(responseString.begin(), responseString.end());
}

void Worker::setAction(Action action)
{
    myAction = action;
}

void Worker::listen()
{
    bool worked = false;

    switch(myAction)
    {
        case IDLE: {
            busy = false;
            break;
        }

        case GET_SERVERS: {
            busy = true;
            getAvailableServers();
            worked = true;
            break;
        }
    }

    if(worked)
        myAction = IDLE;
}

bool Worker::isBusy()
{
    return busy;
}