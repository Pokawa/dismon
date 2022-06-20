add_executable(obinary_stream_test obinary_stream.cpp)
target_link_libraries(obinary_stream_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(obinary_stream_test)

add_executable(ibinary_stream_test ibinary_stream.cpp)
target_link_libraries(ibinary_stream_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(ibinary_stream_test)

add_executable(serialization_test serialization.cpp)
target_link_libraries(serialization_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(serialization_test)

add_executable(serializable_test serializable.cpp)
target_link_libraries(serializable_test GTest::gtest GTest::gtest_main DisMon)
gtest_discover_tests(serializable_test)
