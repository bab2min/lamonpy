#pragma once
#include <string>
#include <memory>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
namespace lamon
{
	namespace utils
	{
		class HandleGuard
		{
			HANDLE handle;
		public:
			HandleGuard(HANDLE _handle = nullptr) : handle(_handle)
			{
			}

			HandleGuard(const HandleGuard&) = delete;
			HandleGuard& operator =(const HandleGuard&) = delete;

			HandleGuard(HandleGuard&& o)
			{
				std::swap(handle, o.handle);
			}

			HandleGuard& operator=(HandleGuard&& o)
			{
				std::swap(handle, o.handle);
				return *this;
			}

			~HandleGuard()
			{
				if (handle && handle != INVALID_HANDLE_VALUE)
				{
					CloseHandle(handle);
					handle = nullptr;
				}
			}

			operator HANDLE() const
			{
				return handle;
			}
		};

		class MMap
		{
			const char* view = nullptr;
			size_t len = 0;
			HandleGuard hFile, hFileMap;
		public:
			MMap(const std::string& filepath)
			{
				hFile = CreateFileA(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
				if (hFile == INVALID_HANDLE_VALUE) throw std::ios_base::failure("Cannot open '" + filepath + "'");
				hFileMap = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
				if (hFileMap == nullptr) throw std::ios_base::failure("Cannot open '" + filepath + "' Code:" + std::to_string(GetLastError()));
				view = (const char*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
				DWORD high;
				len = GetFileSize(hFile, &high);
				len |= (size_t)high << 32;
			}

			MMap(const MMap&) = delete;
			MMap& operator=(const MMap&) = delete;

			MMap(MMap&&) = default;
			MMap& operator=(MMap&&) = default;

			~MMap()
			{
				UnmapViewOfFile(view);
			}

			const char* get() const { return view; }
			size_t size() const { return len; }
		};
	}
}
#else
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

namespace lamon
{
	namespace utils
	{
		class FDGuard
		{
			int fd;
		public:
			FDGuard(int _fd = 0) : fd(_fd)
			{
			}

			FDGuard(const FDGuard&) = delete;
			FDGuard& operator =(const FDGuard&) = delete;

			FDGuard(FDGuard&& o)
			{
				std::swap(fd, o.fd);
			}

			FDGuard& operator=(FDGuard&& o)
			{
				std::swap(fd, o.fd);
				return *this;
			}

			~FDGuard()
			{
				if (fd && fd != -1)
				{
					close(fd);
					fd = 0;
				}
			}

			operator int() const
			{
				return fd;
			}
		};

		class MMap
		{
			const char* view = nullptr;
			size_t len = 0;
			FDGuard fd;
		public:
			MMap(const std::string& filepath)
			{
				fd = open(filepath.c_str(), O_RDONLY);
				if (fd == -1) throw std::ios_base::failure("Cannot open '" + filepath + "'");
				struct stat sb;
				if (fstat(fd, &sb) < 0) throw std::ios_base::failure("Cannot open '" + filepath + "'");
				len = sb.st_size;
				view = (const char*)mmap(nullptr, len, PROT_READ, MAP_SHARED, fd, 0);
				if (view == MAP_FAILED) throw std::ios_base::failure("Mapping failed");
			}

			MMap(const MMap&) = delete;
			MMap& operator=(const MMap&) = delete;

			MMap(MMap&& o)
			{
				std::swap(view, o.view);
				std::swap(fd, o.fd);
			}

			MMap& operator=(MMap&& o)
			{
				std::swap(view, o.view);
				std::swap(fd, o.fd);
				return *this;
			}

			~MMap()
			{
				if (view) munmap((void*)view, len);
				view = nullptr;
			}

			const char* get() const { return view; }
			size_t size() const { return len; }
		};
	}
}
#endif

namespace lamon
{
	namespace utils
	{
		class imstream
		{
		private:
			const char* ptr, * begin, * end;
		public:
			imstream(const MMap& mmap) : ptr(mmap.get()), begin(mmap.get()), end(mmap.get() + mmap.size())
			{
			}

			imstream(const char* _ptr, size_t len) : ptr(_ptr), begin(_ptr), end(_ptr + len)
			{
			}

			template<class _Ty>
			const _Ty& read()
			{
				if (end - ptr < sizeof(_Ty)) throw std::ios_base::failure(std::string{ "reading type '" } + typeid(_Ty).name() + "' failed");
				auto p = (_Ty*)ptr;
				ptr += sizeof(_Ty);
				return *p;
			}

			bool read(void* dest, size_t size)
			{
				if (end - ptr < size) return false;
				std::memcpy(dest, ptr, size);
				ptr += size;
				return true;
			}

			void exceptions(int)
			{
				// dummy functions
			}

			const char* get() const
			{
				return ptr;
			}

			size_t tellg() const
			{
				return ptr - begin;
			}

			bool seekg(std::streamoff distance, std::ios_base::seek_dir dir = std::ios_base::beg)
			{
				if (dir == std::ios_base::beg)
				{
					if (distance < 0 || distance > end - begin) return false;
					ptr = begin + distance;
				}
				else if (dir == std::ios_base::cur)
				{
					if (ptr + distance < begin || ptr + distance > end) return false;
					ptr += distance;
				}
				else if (dir == std::ios_base::end)
				{
					if (distance > 0 || end + distance < begin) return false;
					ptr = end + distance;
				}
				else return false;
				return true;
			}
		};
	}
}
