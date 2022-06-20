#ifndef DISMON_SERIALIZATION_BINARY_DATA_HPP
#define DISMON_SERIALIZATION_BINARY_DATA_HPP

#include <vector>
#include <type_traits>
#include <cstdint>

namespace dismon {

    typedef std::vector<char> binary_data;
    typedef std::uint64_t version_id;

    template<typename T>
    concept fundamental = std::is_fundamental_v<T>;

}

#endif //DISMON_SERIALIZATION_BINARY_DATA_HPP
