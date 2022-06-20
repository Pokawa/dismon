#include "dismon/communication/communication_server.hpp"
#include <chrono>

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    zmq::context_t context;
    std::set<std::string> network;
    for (int i = 2; i < argc; ++i) {
        network.emplace(argv[i]);
    }
    dismon::communication_server server{context, argv[1], network, 9999};
    server.start();

    for(int i = 0; i < 10; ++i) {
        server.ping();
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
