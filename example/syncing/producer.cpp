#include <chrono>
#include <iostream>
#include "distributed_buffer.hpp"

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::warn);

    std::set<std::string> others;
    std::string self{argv[1]};
    for (int i = 2; i < argc - 1; ++i) {
        others.emplace(argv[i]);
    }
    int start = std::atoi(argv[argc - 1]);

    distributed_buffer buffer;
    dismon::monitor monitor{buffer, self, others, 9999};

    for (int i = start; i < start + 100; ++i) {
        monitor.lock();
        while (buffer.full()) {
            monitor.wait("producer");
        }

        buffer.put(i);
        std::cout << i - start << " " << i << " " << buffer.produced << std::endl;
        monitor.notify_one("consumer");
        monitor.unlock();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
