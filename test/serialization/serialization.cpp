#include <gtest/gtest.h>
#include <dismon/serialization/ibinary_stream.hpp>
#include <dismon/serialization/obinary_stream.hpp>

namespace dismon::test {

    TEST(serialization, serializes_integrals) {
        obinary_stream ostream;
        ostream << std::int32_t{1234} << std::int64_t{4321} << std::int8_t{64};

        std::int32_t a;
        std::int64_t b;
        std::int8_t c;
        ibinary_stream{ostream.data()} >> a >> b >> c;

        ASSERT_EQ(a, 1234);
        ASSERT_EQ(b, 4321);
        ASSERT_EQ(c, 64);
    }

    TEST(serialization, serializes_floats) {
        obinary_stream ostream;
        ostream << float{0.25} << double{0.5};

        float a;
        double b;
        ibinary_stream{ostream.data()} >> a >> b;

        ASSERT_EQ(a, 0.25);
        ASSERT_EQ(b, 0.5);
    }

    TEST(serialization, serializes_strings) {
        obinary_stream ostream;
        ostream << "hello world!";

        std::string a;
        ibinary_stream{ostream.data()} >> a;

        ASSERT_EQ(a, "hello world!");
    }

    TEST(serialization, serializes_vectors) {
        obinary_stream ostream;
        std::vector<std::int32_t> vec = {1, 2, 3, 4};
        ostream << vec;

        std::vector<std::int32_t> a;
        ibinary_stream{ostream.data()} >> a;

        ASSERT_EQ(a, vec);
    }

    TEST(serialization, vector_of_strings) {
        obinary_stream ostream;
        std::vector<std::string> in_vec = {"hello", "world", "!!!"};
        ostream << in_vec;

        std::vector<std::string> out_vec;
        ibinary_stream{ostream.data()} >> out_vec;

        ASSERT_EQ(out_vec, in_vec);
    }
}
