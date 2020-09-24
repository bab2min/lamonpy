#pragma once
#include <vector>
#include <array>

namespace lamon
{
	struct Feature
	{
		union
		{
			struct
			{
				uint8_t mood;
				uint8_t tense;
				uint8_t voice;
				uint8_t person;
				uint8_t gender;
				uint8_t number;
				uint8_t case_;
				uint8_t degree;
			};
			std::array<uint8_t, 8> u8;
			uint64_t u64;
		};

		constexpr Feature(
			uint8_t _mood = 0, uint8_t _tense = 0, uint8_t _voice = 0, uint8_t _person = 0,
			uint8_t _gender = 0, uint8_t _number = 0, uint8_t _case = 0, uint8_t _degree = 0
		)
			: mood{ _mood }, tense{ _tense }, voice{ _voice }, person{ _person },
			gender{ _gender }, number{ _number }, case_{ _case }, degree{ _degree }
		{
		}

		constexpr Feature(uint64_t _u64)
			: u64{ _u64 }
		{
		}

		static constexpr Feature moods(uint8_t i)
		{
			return { i, 0, 0, 0, 0, 0, 0, 0 };
		}

		static constexpr Feature tenses(uint8_t i)
		{
			return { 0, i, 0, 0, 0, 0, 0, 0 };
		}

		static constexpr Feature voices(uint8_t i)
		{
			return { 0, 0, i, 0, 0, 0, 0, 0 };
		}

		static constexpr Feature persons(uint8_t i)
		{
			return { 0, 0, 0, i, 0, 0, 0, 0 };
		}

		static constexpr Feature genders(uint8_t i)
		{
			return { 0, 0, 0, 0, i, 0, 0, 0 };
		}

		static constexpr Feature numbers(uint8_t i)
		{
			return { 0, 0, 0, 0, 0, i, 0, 0 };
		}

		static constexpr Feature cases(uint8_t i)
		{
			return { 0, 0, 0, 0, 0, 0, i, 0 };
		}

		static constexpr Feature degrees(uint8_t i)
		{
			return { 0, 0, 0, 0, 0, 0, 0, i };
		}

		uint8_t& operator[](size_t i)
		{
			return u8[i];
		}

		const uint8_t& operator[](size_t i) const
		{
			return u8[i];
		}

		bool operator==(Feature f) const
		{
			return u64 == f.u64;
		}

		bool operator<(Feature f) const
		{
			return u64 < f.u64;
		}

		operator bool() const
		{
			return !!u64;
		}

		Feature operator+(Feature f) const
		{
			return u64 + f.u64;
		}

		void serializerRead(std::istream& istr)
		{
			std::array<uint8_t, 4> buf;
			istr.read((char*)buf.data(), buf.size());
			for (size_t i = 0; i < 4; ++i)
			{
				u8[i * 2] = buf[i] & 0xF;
				u8[i * 2 + 1] = buf[i] >> 4;
			}
		}

		void serializerWrite(std::ostream& ostr) const
		{
			std::array<uint8_t, 4> buf;
			for (size_t i = 0; i < 4; ++i)
			{
				buf[i] = (u8[i * 2] & 0xF) | (u8[i * 2 + 1] << 4);
			}
			ostr.write((const char*)buf.data(), buf.size());
		}
	};
}
