#ifndef CERBERUS_FILE_DOWNLOADER_H
#define CERBERUS_FILE_DOWNLOADER_H
#include <curl/curl.h>
#include <string>

class FileDownloader {
private:
    CURL* curl;
public:
    FileDownloader();
    ~FileDownloader();
    bool download_file(std::string url, std::string path);
};

#endif //CERBERUS_FILE_DOWNLOADER_H
