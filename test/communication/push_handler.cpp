#include <gtest/gtest.h>
#include <dismon/communication/pull_handler.hpp>
#include <dismon/communication/push_handler.hpp>

namespace dismon::test {

    TEST(push_handler, constructs) {
        zmq::context_t context;
        pull_socket socket{context, 9999};
        push_handler handler{context, "127.0.0.1", {}, 9999};
    }

    TEST(push_handler, unicast) {
        zmq::context_t context;
        pull_socket socket{context, 9999};
        push_handler handler{context, "127.0.0.1", {}, 9999};

        handler.unicast(0, MESSAGE_TYPE::PING, {});
        auto m = socket.receive();
        ASSERT_EQ(m.header.type, MESSAGE_TYPE::PING);
    }

    TEST(push_handler, selfcast) {
        zmq::context_t context;
        pull_socket socket{context, 9999};
        push_handler handler{context, "127.0.0.1", {}, 9999};

        handler.selfcast(MESSAGE_TYPE::PING, {});
        auto m = socket.receive();
        ASSERT_EQ(m.header.type, MESSAGE_TYPE::PING);
    }

    TEST(push_handler, multicast) {
        zmq::context_t context;
        pull_socket socket{context, 9999};
        push_handler handler{context, "127.0.0.1", {}, 9999};

        handler.multicast({0}, MESSAGE_TYPE::PING, {});
        auto m = socket.receive();
        ASSERT_EQ(m.header.type, MESSAGE_TYPE::PING);
    }

    TEST(push_handler, broadcast) {
        zmq::context_t context;
        pull_socket socket{context, 9999};
        push_handler handler{context, "127.0.0.1", {}, 9999};

        handler.broadcast(MESSAGE_TYPE::PING, {});
    }
}