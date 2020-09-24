#pragma once

#include <stdexcept>

namespace lamon
{
    namespace exc
    {
        class KeyNotFound : public std::runtime_error
        {
        public:
            using std::runtime_error::runtime_error;
        };

        class ShapeMismatch : public std::runtime_error
        {
        public:
            using std::runtime_error::runtime_error;
        };
    }
}
