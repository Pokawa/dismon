#ifndef DISMON_COMMUNICATION_MESSAGE_HPP
#define DISMON_COMMUNICATION_MESSAGE_HPP

#include <dismon/serialization/types.hpp>
#include <map>
#include <zmq.hpp>
#include <cstdint>

namespace dismon {

    enum class MESSAGE_TYPE : std::uint32_t {
        KILL,
        PING,
        PONG,
        REQUEST,
        REPLY,
        WAIT,
        UNWAIT,
        NOTIFY,
        SYNC,
        STATE,
    };

    typedef std::uint32_t client_id;
    typedef std::uint64_t message_id;


    struct message {
        struct {
            MESSAGE_TYPE type;
            client_id sender;
            message_id id;
        } header;
        binary_data payload;

        static message from_zmq(const zmq::message_t &buffer) {
            auto type = *buffer.data<MESSAGE_TYPE>(); // 0-3 bytes
            auto sender = *(buffer.data<client_id>() + 1); // 4-7 bytes
            auto id = *(buffer.data<message_id>() + 1); // 8-15 bytes
            auto* payload_begin = buffer.data<char>() + sizeof(header); // rest as raw bytes
            auto* payload_end = buffer.data<char>() + buffer.size();

            return {{type, sender, id}, {payload_begin, payload_end}};
        }

        explicit operator zmq::message_t() const {
            zmq::message_t buffer{sizeof(header) + payload.size()};

            *buffer.data<MESSAGE_TYPE>() = header.type;
            *(buffer.data<client_id>() + 1) = header.sender;
            *(buffer.data<message_id>() + 1) = header.id;
            std::ranges::copy(payload, buffer.data<char>() + sizeof(header));

            return buffer;
        }
    };
}

#endif //DISMON_COMMUNICATION_MESSAGE_HPP
