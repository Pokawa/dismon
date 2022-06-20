#include <gtest/gtest.h>
#include <dismon/serialization/ibinary_stream.hpp>
#include <cstring>

namespace dismon::test {

    TEST(ibinary_stream, is_constructed) {
        std::array<char, 4> std_array_buffer{}; // 4 bytes
        ibinary_stream stream1{std_array_buffer};
        ASSERT_EQ(stream1.remaining_length(), 4);

        auto c_array_buffer = std::make_unique<const char[]>(6); // 6 bytes
        ibinary_stream stream2{c_array_buffer.get(), 6};
        ASSERT_EQ(stream2.remaining_length(), 6);

        binary_data binary_buffer(8); // 8 bytes
        ibinary_stream stream3{binary_buffer};
        ASSERT_EQ(stream3.remaining_length(), 8);
    }

    TEST(ibinary_stream, loads_integrals) {
        std::array<char, 13> buffer{};
        *reinterpret_cast<std::int32_t*>(buffer.data()) = 16;
        *reinterpret_cast<std::int64_t*>(buffer.data() + 4) = 32;
        *reinterpret_cast<std::int8_t *>(buffer.data() + 12) = 64;
        ibinary_stream stream{buffer.data(), buffer.size()};

        std::int32_t a;
        std::int64_t b;
        std::int8_t c;
        stream >> a >> b >> c;

        ASSERT_EQ(stream.remaining_length(), 0);
        ASSERT_EQ(a, 16);
        ASSERT_EQ(b, 32);
        ASSERT_EQ(c, 64);
    }

    TEST(ibinary_stream, loads_floats) {
        std::array<char, sizeof(float) + sizeof(double)> buffer{};
        *reinterpret_cast<float *>(buffer.data()) = float(0.25);
        *reinterpret_cast<double *>(buffer.data() + sizeof(float)) = double(0.5);

        ibinary_stream stream{buffer.data(), buffer.size()};

        float a;
        double b;
        stream >> a >> b;

        ASSERT_EQ(stream.remaining_length(), 0);
        ASSERT_EQ(a, 0.25);
        ASSERT_EQ(b, 0.5);
    }

    TEST(ibinary_stream, loads_string) {
        std::array<char, 13> buffer{"hello world!"};
        ibinary_stream stream{buffer};

        std::string a;
        stream >> a;

        ASSERT_EQ(stream.remaining_length(), 0);
        ASSERT_EQ(a, "hello world!");
    }

    TEST(ibinary_stream, loads_vector) {
        std::array<char, sizeof(std::size_t) + sizeof(std::int32_t) * 4> buffer{};
        *reinterpret_cast<std::size_t*>(buffer.data()) = 4;
        *reinterpret_cast<std::int32_t*>(buffer.data() + sizeof(size_t)) = 1;
        *reinterpret_cast<std::int32_t*>(buffer.data() + sizeof(size_t) + 4) = 2;
        *reinterpret_cast<std::int32_t*>(buffer.data() + sizeof(size_t) + 8) = 3;
        *reinterpret_cast<std::int32_t*>(buffer.data() + sizeof(size_t) + 12) = 4;

        ibinary_stream stream{buffer};

        std::vector<std::int32_t> vec;
        stream >> vec;

        ASSERT_EQ(stream.remaining_length(), 0);
        ASSERT_EQ(vec[0], 1);
        ASSERT_EQ(vec[1], 2);
        ASSERT_EQ(vec[2], 3);
        ASSERT_EQ(vec[3], 4);
    }

    TEST(ibinary_stream, loads_std_array) {
        std::array<char, sizeof(std::int32_t) * 4> buffer{};
        *reinterpret_cast<std::int32_t*>(buffer.data()) = 1;
        *reinterpret_cast<std::int32_t*>(buffer.data() + 4) = 2;
        *reinterpret_cast<std::int32_t*>(buffer.data() + 8) = 3;
        *reinterpret_cast<std::int32_t*>(buffer.data() + 12) = 4;

        ibinary_stream stream{buffer};
        std::array<std::int32_t, 4> array{};
        stream >> array;
        ASSERT_EQ(stream.remaining_length(), 0);
        ASSERT_EQ(array[0], 1);
        ASSERT_EQ(array[1], 2);
        ASSERT_EQ(array[2], 3);
        ASSERT_EQ(array[3], 4);
    }

}



