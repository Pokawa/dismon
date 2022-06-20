#include <gtest/gtest.h>
#include <dismon/communication/communication_server.hpp>

namespace dismon::test {

    TEST(communication_server, starts_and_stops) {
        zmq::context_t context;
        communication_server server{context, "127.0.0.1", {}, 9999};
        ASSERT_TRUE(server.is_running());
    }
}
