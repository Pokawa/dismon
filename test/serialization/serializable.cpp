#include <gtest/gtest.h>
#include <dismon/serialization/serializable.hpp>

namespace dismon::test {

    TEST(serializable, compiles) {
        class impl : public serializable {
        public:
            void serialize(obinary_stream &stream) const override {
            }

            void deserialize(ibinary_stream &stream) override {
            }

        };

        obinary_stream ostream;
        ibinary_stream istream{{0}};

        impl object;
        ostream << object;
        istream >> object;
    }

}
