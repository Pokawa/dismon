#ifndef DISMON_DISTRIBUTED_BUFFER_HPP
#define DISMON_DISTRIBUTED_BUFFER_HPP

#include "dismon/monitor.hpp"

struct distributed_buffer : public dismon::serializable {
    std::array<int, 10> buffer = {0};
    std::size_t produced = 0;
    std::size_t producer_index = 0;
    std::size_t consumer_index = 0;

    void put(int value) {
        buffer[producer_index] = value;
        producer_index = (producer_index + 1) % buffer.size();
        produced += 1;
    }

    [[nodiscard]] int get() {
        int val = buffer[consumer_index];
        consumer_index = (consumer_index + 1) % buffer.size();
        produced -= 1;
        return val;
    }

    [[nodiscard]] bool empty() const {
        return produced == 0;
    }

    [[nodiscard]] bool full() const {
        return produced == buffer.size() - 1;
    }

    void serialize(dismon::obinary_stream &stream) const override {
        stream << buffer << produced << producer_index << consumer_index;
    }

    void deserialize(dismon::ibinary_stream &stream) override {
        stream >> buffer >> produced >> producer_index >> consumer_index;
    }

};


#endif //DISMON_DISTRIBUTED_BUFFER_HPP
