#include "Worker.h"
#include "Func.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <chrono>
#include "ResourceManager.h"

using namespace std::chrono;

Worker::Worker()
{

}

#ifdef __linux__
void Worker::platformSpecific()
{
    platform = "linux";
}
#else
void Worker::platformSpecific()
{
    platform = "windows";
}
#endif


void Worker::init()
{
    SPDLOG_INFO("Worker is alive!");
    platformSpecific();

    while(active)
        listen();
}

void Worker::getAvailableServers()
{
    std::string listUrl = "https://dl.patafourgame.com/earthend/essentials/serverList.txt";
    std::string s_servers;

    std::ifstream backup("backupServerList.txt");
    if(backup.good())
    {
        SPDLOG_INFO("Reading servers from backup");
    }
    else
    {
        auto data = downloadFromUrl(listUrl);
        s_servers = std::string(data.begin(), data.end());
        servers = Func::Split(s_servers, '\n');
    }
}

void Worker::testConnection()
{
    std::vector<std::pair<std::string, int>> times;

    for(auto server : servers)
    {
        std::pair<std::string, int> responseTime;

        responseTime.first = server;
        responseTime.second = testUrl(server);

        times.push_back(responseTime);
    }

    std::sort(times.begin(), times.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second < b.second;
    });

    for(auto x : times)
    {
        SPDLOG_INFO("Server: {}, Response: {}ms", x.first, x.second);
    }

    closestServer = times[0].first;
}

size_t Worker::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output, int dl_id) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}
int Worker::ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    Worker* worker = static_cast<Worker*>(clientp); // Cast clientp to Worker*

    if (dltotal > 0) {
        double progress = (double)dlnow / (double)dltotal * 100;
        worker->currentDLProgress = dlnow;      // Store dlnow in the member variable
        worker->currentDLTotal = dltotal;  // Store dltotal in the member variable
        worker->dl_buffer[worker->dl_buffer.size()-1].progress = worker->currentDLProgress;
        worker->dl_buffer[worker->dl_buffer.size()-1].size = worker->currentDLTotal;
        //SPDLOG_INFO("Download Progress: {:.2f}%", progress);
    } else {
        //SPDLOG_INFO("Download Progress: 0%");
    }
    return 0; // Returning 0 tells libcurl to continue downloading
}

int Worker::testUrl(std::string& url)
{
    auto start = high_resolution_clock::now();
    std::string responseString;

    // Initialize CURL
    CURL* curl = curl_easy_init();

    if (curl) {
        CURLcode res;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Worker::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        currentDLProgress = 0;
        currentDLTotal = 0;
    }

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);

    return duration.count();
}

std::vector<char> Worker::downloadFromUrl(std::string url, bool addToBuffer)
{
    SPDLOG_INFO("Attempting to download {}", url);

    std::string responseString;
    File newFile;

    dl_buffer.push_back(newFile);

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
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);

        // Enable progress meter (since CURLOPT_NOPROGRESS is disabled by default in newer versions)
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            SPDLOG_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));

        // Clean up CURL
        curl_easy_cleanup(curl);
    }

    if(addToBuffer)
    {
        files.push_back(std::vector<char>(responseString.begin(), responseString.end()));
    }

    return std::vector<char>(responseString.begin(), responseString.end());
}

std::vector<char> Worker::downloadFromUrlPost(std::string url, std::vector<std::string> postParams, std::vector<std::string> postValues, bool addToBuffer)
{
    SPDLOG_INFO("Attempting to download from {}", url);

    std::ostringstream postStream;

    // Initialize CURL
    CURL* curl = curl_easy_init();

    if(postParams.size() > 0)
    {
        for(auto i=0; i<postParams.size(); i++)
        {
            SPDLOG_INFO("Post param {} value {}", postParams[i], postValues[i]);

            if(i != 0)
                postStream << "&";

            postStream << postParams[i] << "=" << curl_easy_escape(curl, postValues[i].c_str(), postValues[i].length());
        }
    }

    std::string post = postStream.str();

    std::string responseString;
    File newFile;

    dl_buffer.push_back(newFile);

    if (curl) {
        CURLcode res;

        // Set the URL for the HTTPS request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());

        // Use a callback function to capture the response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Worker::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        // Set progress function
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, Worker::ProgressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);

        // Enable progress meter (since CURLOPT_NOPROGRESS is disabled by default in newer versions)
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            SPDLOG_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));

        // Clean up CURL
        curl_easy_cleanup(curl);
    }

    //SPDLOG_INFO("RECEIVED: {}", responseString);

    if(addToBuffer)
    {
        files.push_back(std::vector<char>(responseString.begin(), responseString.end()));
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

        case INIT_FIRSTRUN: {
            busy = true;
            getAvailableServers();
            testConnection();
            downloadFromUrl(closestServer+"earthend/essentials/p4kakupop-pro.ttf", true);
            auto img_data = downloadFromUrl(closestServer+"earthend/essentials/message.png");
            sf::Image img;
            img.loadFromMemory(img_data.data(), img_data.size());
            ResourceManager::getInstance().loadImageAsSprite("resources/graphics/ui/dialog/message.png", img);
            worked = true;
            break;
        }

        case DOWNLOAD_EARTHEND: {
            busy = true;
            auto v = downloadFromUrlPost(closestServer+"getversion.php", {"product_id"}, {"earthend"});
            std::string version = std::string(v.begin(), v.end());
            SPDLOG_INFO("Current Earthend version {}", version);
            auto f = downloadFromUrlPost(closestServer+"getfiles.php",{"product_id", "product_ver", "product_platform"}, {"earthend", version, platform});
            std::string content = std::string(f.begin(), f.end());
            SPDLOG_INFO("Files: {}", content);
            worked = true;
            break;
        }
    }

    if(worked)
        myAction = IDLE;

    std::this_thread::sleep_for(100ms);
}

bool Worker::isBusy()
{
    return busy;
}