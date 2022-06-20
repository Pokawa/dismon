#ifndef DISMON_COMMUNICATION_PULL_HANDLER_HPP
#define DISMON_COMMUNICATION_PULL_HANDLER_HPP

#include <map>
#include <mutex>
#include <thread>
#include <functional>
#include <zmq.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ranges.h>
#include <dismon/communication/message.hpp>

namespace dismon {

    class pull_socket : protected zmq::socket_t {
    public:
        pull_socket(zmq::context_t &context, std::uint16_t port) : zmq::socket_t(context, zmq::socket_type::pull) {
            auto connection_string = fmt::format("tcp://*:{}", port);
            zmq::socket_t::bind(connection_string);
        }

        message receive() {
            zmq::message_t buffer;
            auto received = zmq::socket_t::recv(buffer, zmq::recv_flags::none);
            return message::from_zmq(buffer);
        }
    };

    typedef std::function<void(const message &)> action_callback;

    class pull_handler {
        pull_socket socket;
        std::unique_ptr<std::thread> thread_handle;
        std::map<MESSAGE_TYPE, action_callback> callbacks;
        bool running{false};

    public:
        pull_handler(zmq::context_t &context, std::uint16_t port) : socket(context, port) {}

        ~pull_handler() {
            if (thread_handle) {
                thread_handle->join();
            }
        }

        void on_message(MESSAGE_TYPE type, action_callback callback) {
            callbacks[type] = std::move(callback);
        }

        void start() {
            if (not running) {
                spdlog::trace("Starting pull handler");
                running = true;
                thread_handle = std::make_unique<std::thread>(&pull_handler::run, this);
            }
        }

        [[nodiscard]] bool is_running() const {
            return running;
        }

        void run() {
            while (running) {
                auto request = socket.receive();
                apply_action(request);
            }
        }

    private:
        void apply_action(const message &m) {
            spdlog::trace("RECEIVED <- [{}] : type={} id={} payload=[{}B]", m.header.sender, int(m.header.type),
                          m.header.id, m.payload.size());
            if (m.header.type == MESSAGE_TYPE::KILL) {
                running = false;
                spdlog::trace("Stopping pull handler");
            } else {
                auto &callback = callbacks.at(m.header.type);
                callback(m);
            }
        }
    };

}

#endif //DISMON_COMMUNICATION_PULL_HANDLER_HPP
