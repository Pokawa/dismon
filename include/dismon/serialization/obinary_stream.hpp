#ifndef DISMON_SERIALIZATION_OBINARY_STREAM_HPP
#define DISMON_SERIALIZATION_OBINARY_STREAM_HPP

#include <dismon/serialization/types.hpp>
#include <string_view>
#include <array>
#include <type_traits>

namespace dismon {

    class obinary_stream {
    private:
        binary_data buffer;

    public:
        obinary_stream() : obinary_stream(512) {}

        explicit obinary_stream(std::size_t size) {
            buffer.reserve(size);
        }

        static binary_data from_string(const std::string_view& var) {
            return (obinary_stream() << var).data();
        }

        binary_data &&data() {
            buffer.shrink_to_fit();
            return std::move(buffer);
        }

        obinary_stream &operator<<(const fundamental auto &var) {
            auto *begin = reinterpret_cast<const char *>(&var);
            auto *end = begin + sizeof(var);
            buffer.insert(buffer.end(), begin, end);
            return *this;
        }

        obinary_stream &operator<<(const std::string_view &var) {
            auto *begin = var.begin();
            auto *end = begin + var.length();
            buffer.insert(buffer.end(), begin, end);
            buffer.insert(buffer.end(), 0); // null termination of string
            return *this;
        }

        template<typename T>
        obinary_stream &operator<<(const std::vector<T> &vec) {
            *this << vec.size();
            for (auto &item: vec) {
                *this << item;
            }
            return *this;
        }

        template<typename T, std::size_t N>
        obinary_stream &operator<<(const std::array<T, N> &array) {
            for (auto &item: array) {
                *this << item;
            }
            return *this;
        }
    };

    binary_data from_string(const std::string_view& data) {
        return (obinary_stream() << data).data();
    }

}

#endif //DISMON_SERIALIZATION_OBINARY_STREAM_HPP
