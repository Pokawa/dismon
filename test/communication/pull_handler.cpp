#include <gtest/gtest.h>
#include <dismon/communication/pull_handler.hpp>
#include <dismon/communication/push_handler.hpp>

namespace dismon::test {

    TEST(pull_handler, constructs) {
        zmq::context_t context;
        pull_handler handler{context, 9999};
        push_socket socket{context, "127.0.0.1", 9999};
    }

    TEST(pull_handler, stops_after_kill) {
        zmq::context_t context;
        pull_handler handler{context, 9999};
        push_socket socket{context, "127.0.0.1", 9999};
        handler.start();
        socket.send({{MESSAGE_TYPE::KILL, 0, 0}, {}});
    }

    TEST(pull_handler, applies_action) {
        zmq::context_t context;
        pull_handler handler{context, 9999};
        push_socket socket{context, "127.0.0.1", 9999};
        handler.on_message(MESSAGE_TYPE::PING, [](const message& m){
            ASSERT_EQ(m.header.type, MESSAGE_TYPE::PING);
        });
        handler.start();
        socket.send({{MESSAGE_TYPE::KILL, 0, 0}, {}});
    }
}