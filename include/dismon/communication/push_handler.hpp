#ifndef DISMON_COMMUNICATION_PUSH_HANDLER_HPP
#define DISMON_COMMUNICATION_PUSH_HANDLER_HPP

#include <set>
#include <map>
#include <mutex>
#include <future>
#include <zmq.hpp>
#include <fmt/format.h>
#include <dismon/communication/message.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ranges.h>

namespace dismon {

    class push_socket : protected zmq::socket_t {
    public:
        push_socket(zmq::context_t &context, const std::string &address, std::uint16_t port) : zmq::socket_t(context,
                                                                                                             zmq::socket_type::push) {
            auto connection_string = fmt::format("tcp://{}:{}", address, port);
            zmq::socket_t::connect(connection_string);
        }

        void send(const message &request) {
            zmq::socket_t::send(static_cast<zmq::message_t>(request), zmq::send_flags::none);
        }
    };

    class push_handler {
    public:
        message_id message_counter{0};
        client_id self_id;

    private:
        std::mutex mutex;
        std::map<client_id, std::shared_ptr<push_socket>> sockets;

    public:
        push_handler(zmq::context_t &context, const std::string &self, const std::set<std::string>& others,
                     std::uint16_t port) {

            std::set<std::string> all{others};
            all.insert(self);
            spdlog::trace("Starting push handler, network of {}", all.size());

            for (client_id id = 0; auto& client: all) {
                if (client == self) {
                    self_id = id;
                }
                sockets[id++] = std::make_shared<push_socket>(context, client, port);
            }
            spdlog::trace("Connected push sockets");
        }

        message_id broadcast(MESSAGE_TYPE type, const binary_data &payload) {
            std::lock_guard guard{mutex};
            auto message_id = ++message_counter;
            for (auto& receiver: sockets) {
                if (receiver.first != self_id) {
                    auto socket = receiver.second;
                    send_message(socket, type, payload);
                }
            }
            spdlog::trace("BROADCAST : type={} id={} payload=[{}B]", int(type), message_id, payload.size());
            return message_id;
        }

        message_id unicast(client_id receiver, MESSAGE_TYPE type, const binary_data &payload) {
            std::lock_guard guard{mutex};
            auto message_id = ++message_counter;
            auto socket = sockets.at(receiver);
            send_message(socket, type, payload);
            spdlog::trace("UNICAST -> [{}] : type={} id={} payload=[{}B]", receiver, int(type), message_id, payload.size());
            return message_id;
        }

        message_id selfcast(MESSAGE_TYPE type, const binary_data& payload) {
            return unicast(self_id, type, payload);
        }

        message_id multicast(const std::set<client_id> &receivers, MESSAGE_TYPE type, const binary_data &payload) {
            std::lock_guard guard{mutex};
            auto message_id = ++message_counter;
            for (auto receiver: receivers) {
                auto socket = sockets.at(receiver);
                send_message(socket, type, payload);
            }
            spdlog::trace("MULTICAST -> [{}] : type={} id={} payload=[{}B]", receivers, int(type), message_id, payload.size());
            return message_id;
        }
            // async implementation
//            std::vector<std::future<void>> handles;
//            for (auto receiver: receivers) {
//                auto socket = sockets.at(receiver);
//                auto handle = std::async(std::launch::async, &push_handler::send_message, this, socket, type, payload);
//                handles.emplace_back(std::move(handle));
//            }
//
//            for (auto& handle: handles) {
//                handle.get();
//            }
//        }

    private:
        void send_message(const std::shared_ptr<push_socket>& socket, MESSAGE_TYPE type, const binary_data &payload) {
            message m = {{type, self_id, message_counter}, payload};
            socket->send(m);
        }
    };

}

#endif //DISMON_COMMUNICATION_PUSH_HANDLER_HPP
