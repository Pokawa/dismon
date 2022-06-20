#ifndef DISMON_SERIALIZATION_IBINARY_STREAM_HPP
#define DISMON_SERIALIZATION_IBINARY_STREAM_HPP

#include <dismon/serialization/types.hpp>
#include <string_view>
#include <string>
#include <type_traits>
#include <array>

namespace dismon {

    class ibinary_stream {
    private:
        const char *buf_iter;
        const char *const buf_end;

    public:
        explicit ibinary_stream(const binary_data &buffer) : buf_iter(buffer.data()),
                                                             buf_end(buf_iter + buffer.size()) {}

        ibinary_stream(const char *buffer, std::size_t length) : buf_iter(buffer), buf_end(buf_iter + length) {}

        template<std::size_t length>
        explicit
        ibinary_stream(const std::array<char, length> &buffer) : buf_iter(buffer.data()), buf_end(buf_iter + length) {}

        std::size_t remaining_length() {
            return buf_end - buf_iter;
        }

        ibinary_stream &operator>>(fundamental auto &var) {
            auto *begin = buf_iter;
            auto *end = begin + sizeof(var);
            auto *dst = reinterpret_cast<char *>(&var);

            std::copy(begin, end, dst);
            buf_iter += sizeof(var);

            return *this;
        }

        ibinary_stream &operator>>(std::string &var) {
            var.assign(buf_iter); // based on null termination
            buf_iter += var.length() + 1;
            return *this;
        }

        template<typename T>
        ibinary_stream &operator>>(std::vector<T> &vec) {
            std::size_t vec_size{0};
            *this >> vec_size;

            for (std::size_t i = 0; i < vec_size; ++i) {
                T item;
                *this >> item;
                vec.push_back(std::move(item));
            }

            return *this;
        }

        template<typename T, std::size_t N>
        ibinary_stream &operator>>(std::array<T, N> &array) {
            for (auto &item: array) {
                *this >> item;
            }
            return *this;
        }
    };

    std::string to_string(const binary_data& data) {
        std::string out;
        ibinary_stream(data) >> out;
        return out;
    }

}

#endif //DISMON_SERIALIZATION_IBINARY_STREAM_HPP
