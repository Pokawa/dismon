#ifndef DISMON_COMMUNICATION_COMMUNICATION_SERVER_HPP
#define DISMON_COMMUNICATION_COMMUNICATION_SERVER_HPP

#include <set>
#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <dismon/communication/message.hpp>
#include <dismon/serialization/obinary_stream.hpp>
#include <dismon/serialization/ibinary_stream.hpp>
#include <dismon/communication/pull_handler.hpp>
#include <dismon/communication/push_handler.hpp>

namespace dismon {

    class communication_server :  public pull_handler, public push_handler {
    public:
        communication_server(zmq::context_t& context, const std::string &self, const std::set<std::string> &other, std::uint16_t port)
                : pull_handler(context, port), push_handler(context, self, other, port) {
            default_actions();
        }

        ~communication_server() {
            if (pull_handler::is_running()) {
                push_handler::selfcast(MESSAGE_TYPE::KILL, {});
            }
        }

        void ping() {
            push_handler::broadcast(MESSAGE_TYPE::PING, {});
        }

    private:
        void default_actions() {
            pull_handler::on_message(MESSAGE_TYPE::PING, [this](const message& m) {
               push_handler::unicast(m.header.sender, MESSAGE_TYPE::PONG, m.payload);
            });

            pull_handler::on_message(MESSAGE_TYPE::PONG, [this](const message& m) {
                spdlog::info("PONG from: {}", m.header.sender);
            });
        }
    };
}

#endif //DISMON_COMMUNICATION_COMMUNICATION_SERVER_HPP
