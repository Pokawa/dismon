#include "dismon/monitor.hpp"

#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::warn);
    std::set<std::string> network;
    for (int i = 2; i < argc; ++i) {
        network.emplace(argv[i]);
    }
    dismon::monitor monitor{argv[1], network, 9999};

    for (int i = 0; i < 10; ++i) {
        monitor.lock();
        std::cout << "i = " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        monitor.unlock();
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
