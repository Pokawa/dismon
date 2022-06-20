add_executable(producer producer.cpp distributed_buffer.hpp)
target_link_libraries(producer DisMon)

add_executable(consumer consumer.cpp distributed_buffer.hpp)
target_link_libraries(consumer DisMon)