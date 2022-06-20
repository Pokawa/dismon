#include <gtest/gtest.h>
#include <dismon/communication/message.hpp>

namespace dismon::test {

    TEST(message, constructs) {
        message m{{MESSAGE_TYPE::PING, 123, 321}, {}};
        ASSERT_EQ(m.header.type, MESSAGE_TYPE::PING);
        ASSERT_EQ(m.header.sender, 123);
        ASSERT_EQ(m.header.id, 321);
        ASSERT_TRUE(m.payload.empty());
    }

    TEST(message, works_with_zmq_message) {
        message m{{MESSAGE_TYPE::PING, 123, 321}, {}};
        auto zm = static_cast<zmq::message_t>(m);
        ASSERT_EQ(sizeof(m.header) + m.payload.size(), zm.size());
        auto m2 = message::from_zmq(zm);

        ASSERT_EQ(m.header.type, m2.header.type);
        ASSERT_EQ(m.header.sender, m2.header.sender);
        ASSERT_EQ(m.header.id, m2.header.id);
        ASSERT_EQ(m.payload, m2.payload);
    }

}