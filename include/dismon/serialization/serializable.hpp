#ifndef DISMON_SERIALIZATION_SERIALIZABLE_HPP
#define DISMON_SERIALIZATION_SERIALIZABLE_HPP

#include <dismon/serialization/obinary_stream.hpp>
#include <dismon/serialization/ibinary_stream.hpp>

namespace dismon {

    class serializable {
    public:
        virtual void serialize(obinary_stream &stream) const = 0;

        virtual void deserialize(ibinary_stream &stream) = 0;
    };

    obinary_stream &operator<<(obinary_stream &stream, const serializable &object) {
        object.serialize(stream);
        return stream;
    }

    ibinary_stream &operator>>(ibinary_stream &stream, serializable &object) {
        object.deserialize(stream);
        return stream;
    }

}

#endif //DISMON_SERIALIZATION_SERIALIZABLE_HPP
