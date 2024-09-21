#include "Worker.h"
#include "Func.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <chrono>
#include <regex>
#include <filesystem>
#include "ResourceManager.h"
#include "CoreManager.h"

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

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

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

// Callback function to write data to std::vector<char>
size_t Worker::WriteCallback(void* contents, size_t size, size_t nmemb, std::vector<char>* data) {
    size_t totalSize = size * nmemb;

    // Log each byte as it's received
    char* receivedData = static_cast<char*>(contents);

    // Append the data to the buffer
    data->insert(data->end(), receivedData, receivedData + totalSize);

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

        if(worker->myAction == Worker::DOWNLOAD_EARTHEND)
        {
            worker->update_files.back().progress = worker->currentDLProgress;
            worker->update_files.back().size = worker->currentDLTotal;
        }
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

    std::vector<char> responseString;
    File newFile;

    dl_buffer.push_back(newFile);

    // Initialize CURL

    bool passed = false;

    while(!passed)
    {
        CURL* curl = curl_easy_init();

        if (curl) {
            CURLcode res;

            // Set the URL for the HTTPS request
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_TRANSFERTEXT, 0L); // Ensure binary mode transfer
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L); // Connection timeout

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
            {
                SPDLOG_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                SPDLOG_WARN("Trying again...");
                continue;
            }
        }

        // Clean up CURL
        curl_easy_cleanup(curl);

        passed = true;
    }

    if(addToBuffer)
    {
        files.push_back(responseString);
    }

    return responseString;
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

    std::vector<char> responseString;
    File newFile;

    dl_buffer.push_back(newFile);

    bool passed = false;

    while(!passed)
    {
        curl = curl_easy_init();

        if (curl) {
            CURLcode res;

            // Set the URL for the HTTPS request
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
            curl_easy_setopt(curl, CURLOPT_TRANSFERTEXT, 0L); // Ensure binary mode transfer
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L); // Connection timeout

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
            {
                SPDLOG_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                SPDLOG_WARN("Trying again...");
                continue;
            }
        }

        // Clean up CURL
        curl_easy_cleanup(curl);

        passed = true;
    }

    //SPDLOG_INFO("RECEIVED: {}", responseString);

    if(addToBuffer)
    {
        files.push_back(responseString);
    }

    return responseString;
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
            currentTaskTotal = 20;

            getAvailableServers();
            currentTaskProgress++;

            testConnection();
            currentTaskProgress++;

            StringRepository* strRepo = CoreManager::getInstance().getStrRepo();

            auto fontfile = downloadFromUrl(closestServer+"earthend/essentials/fonts.txt");
            std::string fontlist = std::string(fontfile.begin(), fontfile.end());
            SPDLOG_INFO("Available languages: {}", fontlist);
            std::vector<std::string> fonts = Func::Split(fontlist,'\n');

            for(auto font : fonts)
            {
                std::vector<std::string> param = Func::Split(font, ',');
                std::string name = param[0];
                std::string fontf = param[1];

                downloadFromUrl(closestServer+"earthend/essentials/"+fontf, true);
                strRepo->LoadFontFromString(name, files.back());
                currentTaskProgress += 3;
            }

            auto img_data = downloadFromUrl(closestServer+"earthend/essentials/message.png");
            sf::Image img;
            img.loadFromMemory(img_data.data(), img_data.size());
            currentTaskProgress++;

            ResourceManager::getInstance().loadImageAsSprite("resources/graphics/ui/dialog/message.png", img);
            // get langs
            auto langfile = downloadFromUrl(closestServer+"earthend/essentials/languages.txt");
            std::string langlist = std::string(langfile.begin(), langfile.end());
            SPDLOG_INFO("Available languages: {}", langlist);
            std::vector<std::string> langs = Func::Split(langlist,'\n');
            for(auto lang:langs)
            {
                std::vector<std::string> param = Func::Split(lang, ',');
                std::string code = param[0];
                std::string name = param[1];
                std::string font = param[2];

                auto flag_img_data = downloadFromUrl(closestServer+"earthend/essentials/lang/"+code+"/"+code+".png");
                sf::Image flag_img;
                flag_img.loadFromMemory(flag_img_data.data(), flag_img_data.size());
                ResourceManager::getInstance().loadImageAsSprite(code+".png", flag_img);

                auto lang_strings_data = downloadFromUrl(closestServer+"earthend/essentials/lang/"+code+"/"+code+".txt");
                std::string lang_strings = std::string(lang_strings_data.begin(), lang_strings_data.end());
                SPDLOG_INFO("Data: {}", lang_strings);

                strRepo->LoadLanguageFromString(code, name, lang_strings);
                strRepo->langToFontMapping[code] = font;

                currentTaskProgress++;
            }

            currentTaskProgress++;

            worked = true;
            break;
        }

        case DOWNLOAD_EARTHEND: {
            busy = true;
            currentTaskTotal = 1000;
            auto v = downloadFromUrlPost(closestServer+"getversion.php", {"product_id"}, {"earthend"});
            std::string version = std::string(v.begin(), v.end());
            SPDLOG_INFO("Current Earthend version {}", version);
            auto f = downloadFromUrlPost(closestServer+"getfiles.php",{"product_id", "product_ver", "product_platform"}, {"earthend", version, platform});
            std::string content = std::string(f.begin(), f.end());
            SPDLOG_INFO("Files: {}", content);

            SPDLOG_INFO("Downloading contents to {}", gamePath);
            auto entryList = Func::Split(content, '\n');

            std::string startDir = "";
            int totalSize = 0;

            for(auto entry : entryList)
            {
                if(entry.find("START_DIR") != std::string::npos)
                {
                    startDir = Func::Split(entry, ':')[1];
                    continue;
                }

                if(entry.find("TOTAL_SIZE") != std::string::npos)
                {
                    totalSize = atoi(Func::Split(entry, ':')[1].c_str());
                    continue;
                }

                auto file = Func::Split(entry, ',');

                std::string f_name = file[0];
                std::string f_hash = file[1];
                int f_size = atoi(file[2].c_str());

                FileEntry n;
                n.name = f_name;
                n.size = f_size;
                n.progress = 0;
                update_files.push_back(n);
            }

            currentTaskTotal = totalSize;

            for(auto x : update_files)
            {
                std::string localPath = std::regex_replace(x.name, std::regex(startDir), "");
                SPDLOG_INFO("Downloading {}, size {} bytes", localPath, x.size);

                auto fdata = downloadFromUrl(closestServer+x.name);
                std::filesystem::path lpath(gamePath+"/"+localPath);
                lpath.remove_filename();
                if(!lpath.empty())
                {
                    SPDLOG_INFO("Creating directory: {}", lpath.string());
                    std::filesystem::create_directories(lpath);
                }

                std::ofstream out(gamePath+"/"+localPath, std::ofstream::binary);
                out.write(fdata.data(), fdata.size());
                out.close();
            }

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