#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <unordered_map>
#include <type_traits>
#include <vector>
#include "text.hpp"

namespace lamon
{
	namespace serializer
	{
		template<typename _Ty> inline void writeToStream(std::ostream& ostr, const _Ty& v);
		template<typename _Ty> inline void readFromStream(std::istream& istr, _Ty& v);
		template<typename _Ty> inline _Ty readFromStream(std::istream& istr);

		class UnfitException : public std::ios_base::failure
		{
			using std::ios_base::failure::failure;
		};

		template<typename _IntTy>
		class VarLenInt
		{
			_IntTy value;
		public:
			explicit VarLenInt(_IntTy _value = 0) : value{ _value }
			{
			}
			
			explicit operator _IntTy() const
			{
				return value;
			}
		};

		using vuint32_t = VarLenInt<uint32_t>;

		inline void writeMany(std::ostream& ostr)
		{
			// do nothing
		}

		template<typename _FirstTy, typename ... _RestTy>
		inline void writeMany(std::ostream& ostr, const _FirstTy& first, _RestTy&&... rest)
		{
			writeToStream(ostr, first);
			writeMany(ostr, std::forward<_RestTy>(rest)...);
		}

		inline void readMany(std::istream& istr)
		{
			// do nothing
		}

		template<typename _FirstTy, typename ... _RestTy>
		inline void readMany(std::istream& istr, _FirstTy& first, _RestTy&&... rest)
		{
			readFromStream(istr, first);
			readMany(istr, std::forward<_RestTy>(rest)...);
		}

		namespace detail
		{
			template<typename _Class, typename _RetTy, typename ..._Args>
			_RetTy test_mf(_RetTy(_Class::*mf)(_Args...))
			{
				return _RetTy{};
			}

			template<typename _Class, typename _RetTy, typename ..._Args>
			_RetTy test_mf_c(_RetTy(_Class::*mf)(_Args...) const)
			{
				return _RetTy{};
			}

			template<typename> struct sfinae_true : std::true_type {};
			template<typename _Ty>
			static auto testSave(int)->sfinae_true<decltype(test_mf_c<_Ty, void, std::ostream&>(&_Ty::serializerWrite))>;
			template<typename _Ty>
			static auto testSave(long)->std::false_type;

			template<typename _Ty>
			static auto testLoad(int)->sfinae_true<decltype(test_mf<_Ty, void, std::istream&>(&_Ty::serializerRead))>;
			template<typename _Ty>
			static auto testLoad(long)->std::false_type;
		}
		template<typename _Ty>
		struct hasSave : decltype(detail::testSave<_Ty>(0)){};

		template<typename _Ty>
		struct hasLoad : decltype(detail::testLoad<_Ty>(0)){};

		template<class _Ty>
		inline typename std::enable_if<std::is_fundamental<_Ty>::value || std::is_enum<_Ty>::value>::type writeToBinStreamImpl(std::ostream& ostr, const _Ty& v)
		{
			if (!ostr.write((const char*)&v, sizeof(_Ty)))
				throw std::ios_base::failure(std::string("writing type '") + typeid(_Ty).name() + std::string("' is failed") );
		}

		template<class _Ty>
		inline typename std::enable_if<std::is_fundamental<_Ty>::value || std::is_enum<_Ty>::value>::type readFromBinStreamImpl(std::istream& istr, _Ty& v)
		{
			if (!istr.read((char*)&v, sizeof(_Ty)))
				throw std::ios_base::failure(std::string("reading type '") + typeid(_Ty).name() + std::string("' is failed") );
		}

		template<class _Ty>
		inline typename std::enable_if<hasSave<_Ty>::value>::type writeToBinStreamImpl(std::ostream& ostr, const _Ty& v)
		{
			v.serializerWrite(ostr);
		}

		template<class _Ty>
		inline typename std::enable_if<hasLoad<_Ty>::value>::type readFromBinStreamImpl(std::istream& istr, _Ty& v)
		{
			v.serializerRead(istr);
		}

		template<class _Ty>
		inline void writeToBinStreamImpl(std::ostream& ostr, const std::vector<_Ty>& v)
		{
			writeToStream<vuint32_t>(ostr, (vuint32_t)v.size());
			for (auto& e : v) writeToStream(ostr, e);
		}

		template<class _Ty>
		inline void readFromBinStreamImpl(std::istream& istr, std::vector<_Ty>& v)
		{
			uint32_t size = (uint32_t)readFromStream<vuint32_t>(istr);
			v.resize(size);
			for (auto& e : v) readFromStream(istr, e);
		}

		template<class _Ty1, class _Ty2>
		inline void writeToBinStreamImpl(std::ostream& ostr, const std::pair<_Ty1, _Ty2>& v)
		{
			writeToStream(ostr, v.first);
			writeToStream(ostr, v.second);
		}

		template<class _Ty1, class _Ty2>
		inline void readFromBinStreamImpl(std::istream& istr, std::pair<_Ty1, _Ty2>& v)
		{
			readFromStream(istr, v.first);
			readFromStream(istr, v.second);
		}

		template<class _KeyTy, class _ValTy>
		inline void writeToBinStreamImpl(std::ostream& ostr, const std::unordered_map<_KeyTy, _ValTy>& v)
		{
			writeToStream<vuint32_t>(ostr, (vuint32_t)v.size());
			for (auto& e : v) writeToStream(ostr, e);
		}

		template<class _KeyTy, class _ValTy>
		inline void readFromBinStreamImpl(std::istream& istr, std::unordered_map<_KeyTy, _ValTy>& v)
		{
			uint32_t size = (uint32_t)readFromStream<vuint32_t>(istr);
			v.clear();
			for (size_t i = 0; i < size; ++i)
			{
				v.emplace(readFromStream<std::pair<_KeyTy, _ValTy>>(istr));
			}
		}

		template<class _Ty, size_t _N>
		inline void writeToBinStreamImpl(std::ostream& ostr, const std::array<_Ty, _N>& v)
		{
			writeToStream<vuint32_t>(ostr, (vuint32_t)v.size());
			for (auto& e : v) writeToStream(ostr, e);
		}

		template<class _Ty, size_t _N>
		inline void readFromBinStreamImpl(std::istream& istr, std::array<_Ty, _N>& v)
		{
			uint32_t size = (uint32_t)readFromStream<vuint32_t>(istr);
			if (_N != size) throw std::ios_base::failure( text::format("the size of array must be %zd, not %zd", _N, size) );
			for (auto& e : v) readFromStream(istr, e);
		}

		template<class _Ty>
		inline void writeToBinStreamImpl(std::ostream& ostr, const std::basic_string<_Ty>& v)
		{
			writeToStream<vuint32_t>(ostr, (vuint32_t)v.size());
			if (!ostr.write((const char*)v.data(), sizeof(_Ty) * v.size()))
				throw std::ios_base::failure( std::string("writing type '") + typeid(_Ty).name() + std::string("' is failed") );
		}

		template<class _Ty>
		inline void readFromBinStreamImpl(std::istream& istr, std::basic_string<_Ty>& v)
		{
			uint32_t size = (uint32_t)readFromStream<vuint32_t>(istr);
			v.resize(size);
			if (!istr.read((char*)v.data(), sizeof(_Ty) * v.size()))
				throw std::ios_base::failure( std::string("reading type '") + typeid(_Ty).name() + std::string("' is failed") );
		}

		template<class _Ty>
		inline typename std::enable_if<std::is_abstract<_Ty>::value>::type writeToBinStreamImpl(std::ostream& ostr, const std::unique_ptr<_Ty>& v)
		{
			_Ty::serializerWrite(v, ostr);
		}

		template<class _Ty>
		inline typename std::enable_if<std::is_abstract<_Ty>::value>::type readFromBinStreamImpl(std::istream& istr, std::unique_ptr<_Ty>& v)
		{
			_Ty::serializerRead(v, istr);
		}

		template<class _Ty>
		inline void writeToBinStreamImpl(std::ostream& ostr, const VarLenInt<_Ty>& v)
		{
			std::array<uint8_t, 10> buf = { 0 };
			size_t size = 1;
			size_t iv = (_Ty)v;
			while (iv >= 128)
			{
				buf[buf.size() - size] = (iv & 0x7F) | (size > 1 ? 0x80 : 0);
				++size;
				iv >>= 7;
			}
			buf[buf.size() - size] = iv | (size > 1 ? 0x80 : 0);
			ostr.write((const char*)&buf[buf.size() - size], size);
		}

		template<class _Ty>
		inline void readFromBinStreamImpl(std::istream& istr, VarLenInt<_Ty>& v)
		{
			size_t iv = 0;
			std::array<uint8_t, 1> buf;
			while (1)
			{
				istr.read((char*)buf.data(), 1);
				iv = (iv << 7) | (buf[0] & 0x7F);
				if (!(buf[0] & 0x80)) break;
			}
			v = VarLenInt<_Ty>{ (_Ty)iv };
		}

		template<typename _Ty> 
		inline void writeToStream(std::ostream& ostr, const _Ty& v)
		{
			return writeToBinStreamImpl(ostr, v);
		}

		template<typename _Ty> 
		inline void readFromStream(std::istream& istr, _Ty& v)
		{
			return readFromBinStreamImpl(istr, v);
		}

		template<typename _Ty>
		inline _Ty readFromStream(std::istream& istr)
		{
			_Ty v;
			readFromBinStreamImpl(istr, v);
			return v;
		}
	}
}

#define DEFINE_SERIALIZER(...) void serializerRead(std::istream& istr)\
{\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
}\
void serializerWrite(std::ostream& ostr) const\
{\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}

#define DEFINE_SERIALIZER_WITH_VERSION(v,...) void serializerRead(lamon::serializer::version_holder<v>, std::istream& istr)\
{\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
}\
void serializerWrite(lamon::serializer::version_holder<v>, std::ostream& ostr) const\
{\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}

#define DEFINE_SERIALIZER_CALLBACK(onRead, ...) void serializerRead(std::istream& istr)\
{\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
	this->onRead();\
}\
void serializerWrite(std::ostream& ostr) const\
{\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}

#define DEFINE_SERIALIZER_AFTER_BASE(base, ...) void serializerRead(std::istream& istr)\
{\
	base::serializerRead(istr);\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
}\
void serializerWrite(std::ostream& ostr) const\
{\
	base::serializerWrite(ostr);\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}

#define DEFINE_SERIALIZER_AFTER_BASE_WITH_VERSION(base, v, ...) void serializerRead(lamon::serializer::version_holder<v> _v, std::istream& istr)\
{\
	base::serializerRead(_v, istr);\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
}\
void serializerWrite(lamon::serializer::version_holder<v> _v, std::ostream& ostr) const\
{\
	base::serializerWrite(_v, ostr);\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}

#define DEFINE_SERIALIZER_BASE_WITH_VERSION(base, v) void serializerRead(lamon::serializer::version_holder<v> _v, std::istream& istr)\
{\
	base::serializerRead(_v, istr);\
}\
void serializerWrite(lamon::serializer::version_holder<v> _v, std::ostream& ostr) const\
{\
	base::serializerWrite(_v, ostr);\
}

#define DEFINE_SERIALIZER_AFTER_BASE_CALLBACK(base, onRead, ...) void serializerRead(std::istream& istr)\
{\
	base::serializerRead(istr);\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
	this->onRead();\
}\
void serializerWrite(std::ostream& ostr) const\
{\
	base::serializerWrite(ostr);\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}

#define DEFINE_SERIALIZER_VIRTUAL(...) virtual void serializerRead(std::istream& istr)\
{\
	lamon::serializer::readMany(istr, __VA_ARGS__);\
}\
virtual void serializerWrite(std::ostream& ostr) const\
{\
	lamon::serializer::writeMany(ostr, __VA_ARGS__);\
}
