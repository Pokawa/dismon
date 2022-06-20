add_executable(message_test message.cpp)
target_link_libraries(message_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(message_test)

add_executable(sockets_test sockets.cpp)
target_link_libraries(sockets_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(sockets_test)

add_executable(handlers_test push_handler.cpp pull_handler.cpp)
target_link_libraries(handlers_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(handlers_test)

add_executable(server_test server.cpp)
target_link_libraries(server_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(server_test)