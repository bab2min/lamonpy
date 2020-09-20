#pragma once
#include <type_traits>
#include <vector>
#include <tuple>
#include <set>
#include <limits>
#include <exception>
#include <string>
#include <iostream>
#include <cstring>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <frameobject.h>
#ifdef MAIN_MODULE
#else
#define NO_IMPORT_ARRAY
#endif
#define PY_ARRAY_UNIQUE_SYMBOL LL_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py
{
	struct UniqueObj
	{
		PyObject* obj;
		UniqueObj(PyObject* _obj = nullptr) : obj(_obj) {}
		~UniqueObj()
		{
			Py_XDECREF(obj);
		}

		UniqueObj(const UniqueObj&) = delete;
		UniqueObj& operator=(const UniqueObj&) = delete;

		UniqueObj(UniqueObj&& o) noexcept
		{
			std::swap(obj, o.obj);
		}

		UniqueObj& operator=(UniqueObj&& o) noexcept
		{
			std::swap(obj, o.obj);
			return *this;
		}

		PyObject* get() const
		{
			return obj;
		}

		PyObject* release()
		{
			auto o = obj;
			obj = nullptr;
			return o;
		}

		operator bool() const
		{
			return !!obj;
		}

		operator PyObject* () const
		{
			return obj;
		}
	};

	template<typename T,
		typename std::enable_if<!std::is_integral<T>::value, int>::type = 0>
		inline T makeObjectToCType(PyObject* obj)
	{
	}

	template<>
	inline std::string makeObjectToCType<std::string>(PyObject* obj)
	{
		const char* str = PyUnicode_AsUTF8(obj);
		if (!str) throw std::bad_exception{};
		return str;
	}

	template<>
	inline float makeObjectToCType<float>(PyObject* obj)
	{
		float d = (float)PyFloat_AsDouble(obj);
		if (d == -1 && PyErr_Occurred()) throw std::bad_exception{};
		return d;
	}

	template<>
	inline double makeObjectToCType<double>(PyObject* obj)
	{
		double d = PyFloat_AsDouble(obj);
		if (d == -1 && PyErr_Occurred()) throw std::bad_exception{};
		return d;
	}

	template<typename T,
		typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline T makeObjectToCType(PyObject* obj)
	{
		long long v = PyLong_AsLongLong(obj);
		if (v == -1 && PyErr_Occurred()) throw std::bad_exception{};
		return (T)v;
	}

	template<typename T>
	inline std::vector<T> makeIterToVector(PyObject* iter)
	{
		UniqueObj item;
		std::vector<T> v;
		while ((item = PyIter_Next(iter)))
		{
			v.emplace_back(makeObjectToCType<T>(item));
		}
		if (PyErr_Occurred())
		{
			throw std::bad_exception{};
		}
		return v;
	}

	template<typename T>
	inline std::vector<std::string> makeIterToStringVector(PyObject* iter)
	{
		UniqueObj item;
		std::vector<std::string> v;
		while ((item = PyIter_Next(iter)))
		{
			v.emplace_back(std::to_string(makeObjectToCType<T>(item)));
		}
		if (PyErr_Occurred())
		{
			throw std::bad_exception{};
		}
		return v;
	}

	template<typename _Ty, typename = void>
	struct ValueBuilder;

	template<typename _Ty>
	inline PyObject* buildPyValue(_Ty&& v)
	{
		return ValueBuilder<
			typename std::remove_const<typename std::remove_reference<_Ty>::type>::type
		>{}(std::forward<_Ty>(v));
	}

	template<typename _Ty>
	struct ValueBuilder<_Ty,
		typename std::enable_if<std::is_integral<_Ty>::value>::type>
	{
		PyObject* operator()(_Ty v)
		{
			return PyLong_FromLongLong(v);
		}
	};

	template<typename _Ty>
	struct ValueBuilder<_Ty,
		typename std::enable_if<std::is_enum<_Ty>::value>::type>
	{
		PyObject* operator()(_Ty v)
		{
			return PyLong_FromLongLong((long long)v);
		}
	};

	template<typename _Ty>
	struct ValueBuilder<_Ty,
		typename std::enable_if<std::is_floating_point<_Ty>::value>::type>
	{
		PyObject* operator()(_Ty v)
		{
			return PyFloat_FromDouble(v);
		}
	};

	template<>
	struct ValueBuilder<std::string>
	{
		PyObject* operator()(const std::string& v)
		{
			return PyUnicode_FromStringAndSize(v.data(), v.size());
		}
	};

	template<>
	struct ValueBuilder<bool>
	{
		PyObject* operator()(bool v)
		{
			return PyBool_FromLong(v);
		}
	};

	template<>
	struct ValueBuilder<PyObject*>
	{
		PyObject* operator()(PyObject* v)
		{
			if (v)
			{
				return v;
			}
			else
			{
				Py_INCREF(Py_None);
				return Py_None;
			}
		}
	};

	template<typename _Ty1, typename _Ty2>
	struct ValueBuilder<std::pair<_Ty1, _Ty2>>
	{
		PyObject* operator()(const std::pair<_Ty1, _Ty2>& v)
		{
			auto ret = PyTuple_New(2);
			size_t id = 0;
			PyTuple_SetItem(ret, id++, buildPyValue(std::get<0>(v)));
			PyTuple_SetItem(ret, id++, buildPyValue(std::get<1>(v)));
			return ret;
		}
	};

	namespace detail
	{
		template<class _T> using Invoke = typename _T::type;

		template<size_t...> struct seq { using type = seq; };

		template<class _S1, class _S2> struct concat;

		template<size_t... _i1, size_t... _i2>
		struct concat<seq<_i1...>, seq<_i2...>>
			: seq<_i1..., (sizeof...(_i1) + _i2)...> {};

		template<class _S1, class _S2>
		using Concat = Invoke<concat<_S1, _S2>>;

		template<size_t _n> struct gen_seq;
		template<size_t _n> using GenSeq = Invoke<gen_seq<_n>>;

		template<size_t _n>
		struct gen_seq : Concat<GenSeq<_n / 2>, GenSeq<_n - _n / 2>> {};

		template<> struct gen_seq<0> : seq<> {};
		template<> struct gen_seq<1> : seq<0> {};

		template <size_t _n, size_t ... _is>
		std::array<char, _n - 1> to_array(const char(&a)[_n], seq<_is...>)
		{
			return { {a[_is]...} };
		}

		template <size_t _n>
		constexpr std::array<char, _n - 1> to_array(const char(&a)[_n])
		{
			return to_array(a, GenSeq<_n - 1>{});
		}

		template <size_t _n, size_t ... _is>
		std::array<char, _n> to_arrayz(const char(&a)[_n], seq<_is...>)
		{
			return { {a[_is]..., 0} };
		}

		template <size_t _n>
		constexpr std::array<char, _n> to_arrayz(const char(&a)[_n])
		{
			return to_arrayz(a, GenSeq<_n - 1>{});
		}
	}

	template<typename ..._Ty>
	struct ValueBuilder<std::tuple<_Ty...>>
	{
		template<int _n>
		void set(PyObject* obj)
		{
		}

		template<int _n, typename _Arg, typename ... _ArgRest>
		void set(PyObject* obj, _Arg&& first, _ArgRest&&... rest)
		{
			PyTuple_SetItem(obj, _n, buildPyValue(std::forward<_Arg>(first)));
			return set<_n + 1>(obj, std::forward<_ArgRest>(rest)...);
		}

		template<typename ..._Tp, size_t ... _is>
		void set_unpack(PyObject* obj, const std::tuple<_Tp...>& v, detail::seq<_is...>)
		{
			return set<0>(obj, std::get<_is>(v)...);
		}

		PyObject* operator()(const std::tuple<_Ty...>& v)
		{
			auto ret = PyTuple_New(sizeof...(_Ty));
			set_unpack(ret, v, detail::GenSeq<sizeof...(_Ty)>{});
			return ret;
		}
	};

	namespace detail
	{
		template<typename _Ty>
		struct NpyType
		{
		};

		template<>
		struct NpyType<int8_t>
		{
			enum {
				type = NPY_INT8,
				signed_type = type
			};
		};

		template<>
		struct NpyType<uint8_t>
		{
			enum {
				type = NPY_UINT8,
				signed_type = NPY_INT8
			};
		};

		template<>
		struct NpyType<int16_t>
		{
			enum {
				type = NPY_INT16,
				signed_type = type
			};
		};

		template<>
		struct NpyType<uint16_t>
		{
			enum {
				type = NPY_UINT16,
				signed_type = NPY_INT16
			};
		};

		template<>
		struct NpyType<int32_t>
		{
			enum {
				type = NPY_INT32,
				signed_type = type
			};
		};

		template<>
		struct NpyType<uint32_t>
		{
			enum {
				type = NPY_UINT32,
				signed_type = NPY_INT32
			};
		};

		template<>
		struct NpyType<int64_t>
		{
			enum {
				type = NPY_INT64,
				signed_type = type
			};
		};

		template<>
		struct NpyType<uint64_t>
		{
			enum {
				type = NPY_UINT64,
				signed_type = NPY_INT64
			};
		};

		template<>
		struct NpyType<float>
		{
			enum {
				type = NPY_FLOAT,
				signed_type = type
			};
		};

		template<>
		struct NpyType<double>
		{
			enum {
				type = NPY_DOUBLE,
				signed_type = type
			};
		};
	}

	struct cast_to_signed_t
	{
	};

	static constexpr cast_to_signed_t cast_to_signed{};

	template<typename _Ty>
	struct ValueBuilder<std::vector<_Ty>,
		typename std::enable_if<std::is_arithmetic<_Ty>::value>::type>
	{
		PyObject* operator()(const std::vector<_Ty>& v)
		{
			npy_intp size = v.size();
			PyObject* obj = PyArray_EMPTY(1, &size, detail::NpyType<_Ty>::type, 0);
			std::memcpy(PyArray_DATA((PyArrayObject*)obj), v.data(), sizeof(_Ty) * size);
			return obj;
		}
	};

	template<typename _Ty>
	struct ValueBuilder<std::vector<_Ty>,
		typename std::enable_if<!std::is_arithmetic<_Ty>::value>::type>
	{
		PyObject* operator()(const std::vector<_Ty>& v)
		{
			auto ret = PyList_New(v.size());
			size_t id = 0;
			for (auto& e : v)
			{
				PyList_SetItem(ret, id++, buildPyValue(e));
			}
			return ret;
		}
	};

	template<typename _Ty>
	inline typename std::enable_if<std::is_arithmetic<_Ty>::value, PyObject*>::type
		buildPyValue(const std::vector<_Ty>& v, cast_to_signed_t)
	{
		npy_intp size = v.size();
		PyObject* obj = PyArray_EMPTY(1, &size, detail::NpyType<_Ty>::signed_type, 0);
		std::memcpy(PyArray_DATA((PyArrayObject*)obj), v.data(), sizeof(_Ty) * size);
		return obj;
	}

	template<typename _Ty>
	inline typename std::enable_if<
		!std::is_arithmetic<typename std::iterator_traits<_Ty>::value_type>::value,
		PyObject*
	>::type buildPyValue(_Ty first, _Ty last)
	{
		auto ret = PyList_New(std::distance(first, last));
		size_t id = 0;
		for (; first != last; ++first)
		{
			PyList_SetItem(ret, id++, buildPyValue(*first));
		}
		return ret;
	}

	template<typename _Ty, typename _Tx>
	inline typename std::enable_if<
		!std::is_arithmetic<
		typename std::result_of<_Tx(typename std::iterator_traits<_Ty>::value_type)>::type
		>::value,
		PyObject*
	>::type buildPyValueTransform(_Ty first, _Ty last, _Tx tx)
	{
		auto ret = PyList_New(std::distance(first, last));
		size_t id = 0;
		for (; first != last; ++first)
		{
			PyList_SetItem(ret, id++, buildPyValue(tx(*first)));
		}
		return ret;
	}


	template<typename _Ty>
	inline typename std::enable_if<
		std::is_arithmetic<typename std::iterator_traits<_Ty>::value_type>::value,
		PyObject*
	>::type buildPyValue(_Ty first, _Ty last)
	{
		using value_type = typename std::iterator_traits<_Ty>::value_type;
		npy_intp size = std::distance(first, last);
		PyObject* ret = PyArray_EMPTY(1, &size, detail::NpyType<value_type>::type, 0);
		size_t id = 0;
		for (; first != last; ++first, ++id)
		{
			*(value_type*)PyArray_GETPTR1((PyArrayObject*)ret, id) = *first;
		}
		return ret;
	}

	template<typename _Ty, typename _Tx>
	inline typename std::enable_if<
		std::is_arithmetic<
		typename std::result_of<_Tx(typename std::iterator_traits<_Ty>::value_type)>::type
		>::value,
		PyObject*
	>::type buildPyValueTransform(_Ty first, _Ty last, _Tx tx)
	{
		using value_type = typename std::iterator_traits<_Ty>::value_type;
		npy_intp size = std::distance(first, last);
		PyObject* ret = PyArray_EMPTY(1, &size, detail::NpyType<value_type>::type, 0);
		size_t id = 0;
		for (; first != last; ++first, ++id)
		{
			*(value_type*)PyArray_GETPTR1((PyArrayObject*)ret, id) = tx(*first);
		}
		return ret;
	}

	namespace dict
	{
		inline void setDictItem(PyObject* dict, const char** keys)
		{

		}

		template<typename _Ty, typename... _Rest>
		inline void setDictItem(PyObject* dict, const char** keys, _Ty&& value, _Rest&& ... rest)
		{
			{
				UniqueObj v = buildPyValue(value);
				PyDict_SetItemString(dict, keys[0], v);
			}
			return setDictItem(dict, keys + 1, std::forward<_Rest>(rest)...);
		}

		template<typename _Ty>
		inline bool isNull(_Ty v)
		{
			return false;
		}

		template<>
		inline bool isNull<PyObject*>(PyObject* v)
		{
			return !v;
		}

		inline void setDictItemSkipNull(PyObject* dict, const char** keys)
		{

		}

		template<typename _Ty, typename... _Rest>
		inline void setDictItemSkipNull(PyObject* dict, const char** keys, _Ty&& value, _Rest&& ... rest)
		{
			if (!isNull(value))
			{
				UniqueObj v = buildPyValue(value);
				PyDict_SetItemString(dict, keys[0], v);
			}
			return setDictItemSkipNull(dict, keys + 1, std::forward<_Rest>(rest)...);
		}
	}

	template<typename... _Rest>
	inline PyObject* buildPyDict(const char** keys, _Rest&&... rest)
	{
		PyObject* dict = PyDict_New();
		dict::setDictItem(dict, keys, std::forward<_Rest>(rest)...);
		return dict;
	}

	template<typename... _Rest>
	inline PyObject* buildPyDictSkipNull(const char** keys, _Rest&&... rest)
	{
		PyObject* dict = PyDict_New();
		dict::setDictItemSkipNull(dict, keys, std::forward<_Rest>(rest)...);
		return dict;
	}

	template<typename _Ty>
	inline void setPyDictItem(PyObject* dict, const char* key, _Ty&& value)
	{
		UniqueObj v = buildPyValue(value);
		PyDict_SetItemString(dict, key, v);
	}
}
