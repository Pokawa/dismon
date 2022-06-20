#include <chrono>
#include <iostream>

#include "distributed_buffer.hpp"

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::warn);

    std::set<std::string> others;
    std::string self{argv[1]};
    for (int i = 2; i < argc; ++i) {
        others.emplace(argv[i]);
    }

    distributed_buffer buffer;
    dismon::monitor monitor{buffer, self, others, 9999};

    for (int i = 0; i < 100; ++i) {
        monitor.lock();
        while (buffer.empty()) {
            monitor.wait("consumer");
        }
        std::cout << i << " " << buffer.get() << " " << buffer.produced << std::endl;
        monitor.notify_one("producer");
        monitor.unlock();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
