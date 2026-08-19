#ifndef UPLOADER_H
#define UPLOADER_H

#include <atomic>
#include <string>
#include <thread>

#include "collector.h"
#include "json_serializer.h"

class Uploader {
   private:
    Collector& collector;  // Collector object to grab all the data
    std::string
        host;  // The server address which is where this server is being run on
               // set to local host
               // so that the http request is sent // internally and not to an external server
    int port;  // The network number which reserve the port for springboot
    std::string path;  // Path end for the http url that springboot can read from

    std::atomic<bool> running_thread{false};
    std::thread uploader_thread;
    JsonSerializer serializer;

   public:
    Uploader(Collector& collector, std::string host, int port, std::string path);
    ~Uploader();
    void run_thread();
};

#endif