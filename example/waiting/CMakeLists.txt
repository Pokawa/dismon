add_executable(waiter waiter.cpp)
target_link_libraries(waiter DisMon)

add_executable(notifier notifier.cpp)
target_link_libraries(notifier DisMon)