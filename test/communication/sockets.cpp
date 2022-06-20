#include <gtest/gtest.h>
#include <dismon/communication/pull_handler.hpp>
#include <dismon/communication/push_handler.hpp>

namespace dismon::test {

    TEST(sockets, constructs) {
        zmq::context_t context;
        pull_socket s2{context, 9999};
        push_socket s1{context, "127.0.0.1", 9999};
    }

    TEST(sockets, sends) {
        zmq::context_t context;
        pull_socket s2{context, 9999};
        push_socket s1{context, "127.0.0.1", 9999};

        s1.send({{MESSAGE_TYPE::PING, 123, 321}, {}});
        auto m = s2.receive();
        ASSERT_EQ(m.header.type, MESSAGE_TYPE::PING);
        ASSERT_EQ(m.header.sender, 123);
        ASSERT_EQ(m.header.id, 321);
    }
}

