#include "server_uploader.h"

#include <chrono>
#include <memory>

#include "httplib.h"

// Intitialize variables and set thread running to true
Uploader::Uploader(Collector& collector, std::string host, int port, std::string path)
    : collector(collector) {
    this->host = host;
    this->port = port;
    this->path = path;

    running_thread = true;
    // Start the uploader thread
    uploader_thread = std::thread(&Uploader::run_thread, this);
}

// Tell the main thread to wait until the uploader_thread has
// finished executing which it will because running_thread is set to
// false stopping the collector thread loop
Uploader::~Uploader() {
    running_thread = false;
    uploader_thread.join();
}

void Uploader::run_thread() {
    // Constructs the http client object with a server address and network port
    httplib::Client client(host, port);

    // continuously loop sending a
    auto next_cycle = std::chrono::steady_clock::now();
    while (running_thread) {
        next_cycle += std::chrono::milliseconds(100);

        // Grab the data and serialize it into .json syntax
        std::shared_ptr<const StatisticsData> monitor_data = collector.get_monitor_data();
        std::string serialized_data = serializer.serialize_snapshot(*monitor_data);
        // Make a tcp connection with port 8080 and sends the .json string thorugh it for springboot
        // to collect Path is where its going, serialized_data is the .json string, and
        // "application/json" describes the data so that springboot can automatically convert it
        client.Post(path, serialized_data, "application/json");

        // Allow the main thread and collector thread to run while this thread waits for 100ms to
        // fully pass
        std::this_thread::sleep_until(next_cycle);
    }
}