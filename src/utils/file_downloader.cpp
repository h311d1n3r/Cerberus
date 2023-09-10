#include <utils/file_downloader.h>

using namespace std;

FileDownloader::FileDownloader() {
    curl = curl_easy_init();
}

FileDownloader::~FileDownloader() {
    curl_easy_cleanup(curl);
}

size_t write_callback(void* contents, size_t size, size_t nmemb, FILE* file) {
    return fwrite(contents, size, nmemb, file);
}

bool FileDownloader::download_file(string url, string path) {
    FILE* file = fopen(path.c_str(), "wb");
    if (file) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        CURLcode res = curl_easy_perform(curl);
        fclose(file);
        if (res == CURLE_OK) return true;
    }
    return false;
}