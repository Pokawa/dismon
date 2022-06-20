#include "dismon/monitor.hpp"

#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    std::set<std::string> network;
    for (int i = 3; i < argc; ++i) {
        network.emplace(argv[i]);
    }
    dismon::monitor monitor{argv[2], network, 9999};

    monitor.lock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    monitor.wait("1");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    monitor.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
