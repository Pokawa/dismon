#include <gtest/gtest.h>
#include <dismon/serialization/obinary_stream.hpp>

namespace dismon::test {

    TEST(obinary_stream, pushes_integrals) {
        obinary_stream stream;
        std::size_t expected_size = 0;

        stream << std::int32_t{1};
        expected_size += sizeof(std::int32_t);

        stream << std::int64_t{2};
        expected_size += sizeof(std::int64_t);

        stream << std::uint32_t{3};
        expected_size += sizeof(std::uint32_t);

        stream << std::uint64_t{4};
        expected_size += sizeof(std::uint64_t);

        stream << std::int8_t{'A'};
        expected_size += sizeof(std::int8_t);

        ASSERT_EQ(stream.data().size(), expected_size);
    }

    TEST(obinary_stream, pushes_floats) {
        obinary_stream stream;
        std::size_t expected_size = 0;

        stream << float{1.001};
        expected_size += sizeof(float);

        stream << double{2.002};
        expected_size += sizeof(double);

        ASSERT_EQ(stream.data().size(), expected_size);
    }

    TEST(obinary_stream, pushes_std_vector) {
        std::vector<std::int32_t> vector(10, 1);
        obinary_stream stream;
        stream << vector;
        auto expected_size = sizeof(std::size_t) + 10 * sizeof(std::int32_t);
        ASSERT_EQ(stream.data().size(), expected_size);
    }

    TEST(obinary_stream, pushes_std_array) {
        std::array<std::int32_t, 10> array{};
        obinary_stream stream;
        stream << array;
        auto expected_size = 10 * sizeof(std::int32_t);
        ASSERT_EQ(stream.data().size(), expected_size);
    }

    TEST(obinary_stream, pushes_string) {
        std::string str{"hello world!"};
        obinary_stream stream;
        stream << str;

        auto expected_size = str.length() + 1;
        ASSERT_EQ(stream.data().size(), expected_size);
    }

    TEST(obinary_stream, pushes_c_string) {
        std::string str{"hello world!"};
        obinary_stream stream;
        stream << str.c_str();

        auto expected_size = str.length() + 1;
        ASSERT_EQ(stream.data().size(), expected_size);
    }

    TEST(obinary_stream, getting_data_resets_buffer) {
        obinary_stream stream;
        stream << 1000;
        ASSERT_NE(stream.data().size(), 0);

        auto data = stream.data();
        ASSERT_EQ(stream.data().size(), 0);

        stream << 2000;
        ASSERT_NE(stream.data().size(), 0);
    }

}




