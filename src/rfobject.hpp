#pragma once

#include <array>
#include <unordered_map>
#include <Eigen/Dense>

#include "rfexception.hpp"
#include "mmap.hpp"

namespace lamon
{
    template<typename _Ty = float>
    using ConstMatrix = Eigen::Map<const Eigen::Matrix<_Ty, -1, -1>, Eigen::Aligned64>;

    template<typename _Ty = float>
    using ConstVector = Eigen::Map<const Eigen::Matrix<_Ty, -1, 1>, Eigen::Aligned64>;

    namespace utils
    {
        struct RFHeader
        {
            std::array<char, 4> magic; // 고유 아이디
            uint32_t header_size; // 헤더 사이즈
            uint64_t cont_size; // 헤더 포함 전체 사이즈
            uint32_t rank; // 랭크
            std::vector<uint32_t> shape; // rank길이만큼의 shape
            std::string name; // 이름
            // paddings...
        };

        class ObjectCollection;

        class Object
        {
            friend ObjectCollection;
            const void* _base = nullptr;
            size_t _size = 0;
            std::vector<uint32_t> _shape;

            static std::pair<std::string, Object> read_one(lamon::utils::imstream& is)
            {
                size_t start_pos = is.tellg();
                std::array<char, 4> magic;
                try
                {
                    magic = is.read<std::array<char, 4>>();
                }
                catch (const std::ios_base::failure&)
                {
                    return std::make_pair(std::string{}, Object{});
                }

                if (magic != std::array<char, 4>{'R', 'F', 'M', 'F'})
                    throw std::ios_base::failure{ "Wrong format" };
                uint32_t header_size = is.read<uint32_t>();
                uint64_t cont_size = is.read<uint64_t>();
                uint32_t rank_size = is.read<uint32_t>();

                Object obj;
                obj._shape.resize(rank_size);
                is.read(obj._shape.data(), sizeof(uint32_t) * rank_size);
                std::string name{ is.get() };
                is.seekg(start_pos + header_size);
                obj._base = is.get();
                obj._size = cont_size;
                is.seekg(start_pos + cont_size);
                return std::make_pair(name, obj);
            }

            Object()
            {
            }
        public:
            Object(const Object&) = default;
            Object(Object&&) = default;

            Object& operator=(const Object&) = default;
            Object& operator=(Object&&) = default;

            template<typename _Ty>
            const _Ty* ptr() const
            {
                return reinterpret_cast<const _Ty*>(_base);
            }

            size_t size() const { return _size; }

            const std::vector<uint32_t>& shape() const { return _shape; }

            template<typename _Ty = float>
            ConstVector<_Ty> to_vector() const
            {
                if (shape().size() != 1) throw exc::ShapeMismatch{ "cannot convert to vector" };
                return ConstVector<_Ty>{ ptr<_Ty>(), (Eigen::Index)shape()[0] };
            }

            template<typename _Ty = float>
            ConstMatrix<_Ty> to_matrix() const
            {
                if (shape().size() != 2) throw exc::ShapeMismatch{ "cannot convert to matrix" };
                return ConstMatrix<_Ty>{ ptr<_Ty>(), (Eigen::Index)shape()[0], (Eigen::Index)shape()[1] };
            }

            template<typename _Ty = float>
            ConstMatrix<_Ty> to_matrix(size_t last_index) const
            {
                if (shape().size() != 3) throw exc::ShapeMismatch{ "cannot convert to matrix" };
                size_t stride = shape()[0] * shape()[1];
                return ConstMatrix<_Ty>{ ptr<_Ty>() + stride * last_index, (Eigen::Index)shape()[0], (Eigen::Index)shape()[1] };
            }
        };

        class ObjectCollection : public std::unordered_map<std::string, Object>
        {
        public:
            const Object& operator[](const std::string& key) const
            {
                auto it = this->find(key);
                if (it == this->end()) throw exc::KeyNotFound{ "Key '" + key + "' not found." };
                return it->second;
            }

            static ObjectCollection read_from(const lamon::utils::MMap& mm)
            {
                ObjectCollection ret;
                lamon::utils::imstream is{ mm };
                for (auto r = Object::read_one(is); !r.first.empty(); r = Object::read_one(is))
                {
                    ret.emplace(r);
                }
                return ret;
            }
        };
    }
}
